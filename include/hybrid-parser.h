#pragma once

#include <cstddef>
#include <cstdint>
#include <immintrin.h>
#include "sse-utils.h"

namespace scalar {

    template<int N>
    uint32_t convert(const char* s, uint32_t prev) {
        return convert<N - 1>(s + 1, prev * 10 + uint8_t(s[0]) - '0');
    }

    template<>
    uint32_t convert<0>(const char* /*s*/, uint32_t prev) {
        return prev;
    }

    template<int N>
    uint32_t convert(const char* s) {
        return convert<N>(s, 0);
    }

}

template <typename MATCHER, typename INSERTER>
void hybrid_parser(const char* string, size_t size, const char* separators, MATCHER matcher, INSERTER output) {
    char* data = const_cast<char*>(string);
    char* end  = data + size;
    bool has_last = false;
    uint32_t val = 0;
    while (data + 16 < end) {
        const __m128i  input = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));
        const __m128i  t0 = sse::decimal_digits_mask(input);
        const uint16_t digit_mask = _mm_movemask_epi8(t0);
        const uint16_t valid_mask = _mm_movemask_epi8(matcher.get_mask(input, t0));

        if (valid_mask != 0xffff) {
            throw std::runtime_error("Wrong character");
        }

        if (digit_mask == 0) {
            data += 16;
            continue;
        }

        switch (digit_mask & 0xff) {
            #include "hybrid-parser.inl"
        }

        data += 8;

        switch (digit_mask >> 8) {
            #include "hybrid-parser.inl"
        }

        data += 8;
    } // for

    // process the tail
    scalar::parse_unsigned(data, string + size - data, separators, output);
}
