#pragma once

#include <cstddef>
#include <cstdint>
#include <immintrin.h>
#include "sse/sse-utils.h"
#include "scalar/scalar-parse-signed.h"

namespace hybrid_signed {

    template<int N>
    uint32_t convert(const char* s, uint32_t prev) {
        int8_t digit = int8_t(s[0]) - '0';
        if (digit < 0) {
            throw std::runtime_error("'+' or '-' on a wrong position");
        }
        return convert<N - 1>(s + 1, prev * 10 + digit);
    }

    template<>
    uint32_t convert<0>(const char* /*s*/, uint32_t prev) {
        return prev;
    }

    template<int N>
    int32_t convert(const char* s) {
        if (s[0] == '+')
            return convert<N - 1>(s + 1, 0);
        else if (s[0] == '-') // TODO: check range
            return -static_cast<int32_t>(convert<N - 1>(s + 1, 0));
        else
            return convert<N>(s, 0);
    }

}

template <typename MATCHER, typename INSERTER>
void parser_hybrid_signed(const char* string, size_t size, const char* separators, MATCHER matcher, INSERTER output) {

    #include "hybrid-shift-back.inl"

    const __m128i ascii_plus  = _mm_set1_epi8('+');
    const __m128i ascii_minus = _mm_set1_epi8('-');

    char* data = const_cast<char*>(string);
    char* end  = data + size;

    enum Previous {
        none,
        has_sign,
        has_value
    };

    Previous prev = none;
    bool    negative = false;
    int32_t val = 0;

    uint16_t span_mask = 0;
    while (data + 16 < end) {
        const __m128i  input = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));
        const __m128i  bytemask_digit = sse::decimal_digits_mask(input);
        const __m128i  bytemask_plus  = _mm_cmpeq_epi8(input, ascii_plus);
        const __m128i  bytemask_minus = _mm_cmpeq_epi8(input, ascii_minus);
        const __m128i  bytemask_sign  = _mm_or_si128(bytemask_minus, bytemask_plus);
        const __m128i  bytemask_span  = _mm_or_si128(bytemask_digit, bytemask_sign);
        const uint16_t valid_mask = _mm_movemask_epi8(matcher.get_mask(input, bytemask_span));

        if (valid_mask != 0xffff) {
            throw std::runtime_error("Wrong character");
        }

        span_mask = _mm_movemask_epi8(bytemask_span);
        if (span_mask == 0) {
            data += 16;
            continue;
        }

        switch (span_mask & 0xff) {
            #include "hybrid-parser-signed.inl"
        }

        data += 8;

        switch (span_mask >> 8) {
            #include "hybrid-parser-signed.inl"
        }

        data += 8;
    } // for

    // Shift back if the last span in the last wasn't saved yet
    // XXX: If there is a really long sequence of digits (more
    //      than 16) then this fixup will not help.
    if (prev != none) {
        data -= shift[span_mask >> 8];
    }

    // process the tail
    scalar::parse_signed(data, string + size - data, separators, output);
}
