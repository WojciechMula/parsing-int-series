#pragma once

#include <cassert>
#include <cstring>

#include <immintrin.h>

#include "scalar/scalar-parse-signed.h"
#include "sse/sse-parser-signed.h"

#include "block_info.h"

namespace avx512 {

    enum Lookup: uint8_t {

        DIGIT   = 0x80,
        SIGN    = 0xc0,
        VALID   = 1,
        INVALID = 0
    };

    void prepare_lookup(const char* separators, uint8_t result[128]) {
        uint8_t* c = (uint8_t*)(separators);

        memset(result, INVALID, 128);

        for (int i='0'; i <= '9'; i++) {
            result[i] = DIGIT;
        }

        result['-'] = SIGN;
        result['+'] = SIGN;

        while (*c) {
            uint8_t x = *c++;
            if (x & 0x80) {
                throw std::logic_error("extended ASCII is not supported");
            }

            switch (x) {
                case '0': case '1': case '2':
                case '3': case '4': case '5':
                case '6': case '7': case '8':
                case '9': case '+': case '-':
                    throw std::logic_error("digits and sign chars are reserved");
            }

            result[x] = VALID;
        }
    }

    template <typename INSERTER>
    void parser_signed(const char* string, size_t size, const char* separators, INSERTER output) {

        char* data = const_cast<char*>(string);
        char* end  = data + size;

        uint8_t classes_lookup[128];
        prepare_lookup(separators, classes_lookup);

        const __m512i class_lo = _mm512_loadu_si512(reinterpret_cast<__m512i*>(&classes_lookup[0]));
        const __m512i class_hi = _mm512_loadu_si512(reinterpret_cast<__m512i*>(&classes_lookup[64]));
        while (data + 64 < end) {
            const __m512i input = _mm512_loadu_si512(reinterpret_cast<__m512i*>(data));

            const __m512i classes = _mm512_permutex2var_epi8(class_lo, input, class_hi);

            if (_mm512_test_epi8_mask(classes, classes) != uint64_t(-1)) {
                throw std::logic_error("invalid character");
            }

            uint64_t span_mask64 = _mm512_movepi8_mask(classes);
            uint64_t sign_mask64 = _mm512_test_epi8_mask(classes, _mm512_set1_epi8(int8_t(0x40)));

            char* bufend = data + 64;
            while (data + 16 <= bufend) {
                const uint16_t span_mask = span_mask64 & 0xffff;
                const uint16_t sign_mask = sign_mask64 & 0xffff;

                const BlockInfo& bi = blocks[span_mask];
                if (sign_mask & bi.invalid_sign_mask) {
                    throw std::runtime_error("'+' or '-' at invalid position");
                }

                const __m128i chunk = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));

                const __m128i shuffle_digits = _mm_loadu_si128((const __m128i*)bi.shuffle_digits);
                const __m128i shuffle_signs  = _mm_loadu_si128((const __m128i*)bi.shuffle_signs);

                const __m128i shuffled    = _mm_shuffle_epi8(chunk, shuffle_digits);
                const __m128i negate_mask = _mm_cmpeq_epi8(_mm_shuffle_epi8(chunk, shuffle_signs), _mm_set1_epi8('-'));
                if (bi.conversion == Conversion::SSE1Digit) {

                    sse::convert_1digit(shuffled, bi.element_count, output);

                } else if (bi.conversion == Conversion::SSE2Digits) {

                    sse::convert_2digits_signed(shuffled, negate_mask, bi.element_count, output);

                } else if (bi.conversion == Conversion::SSE4Digits) {

                    sse::convert_4digits_signed(shuffled, negate_mask, bi.element_count, output);

                } else if (bi.conversion == Conversion::SSE8Digits) {

                    sse::convert_8digits_signed(shuffled, negate_mask, bi.element_count, output);

                } else {

                    printf("case %04x not handled yet\n", span_mask);
                    assert(false);
                }

                data += bi.total_skip;

                span_mask64 >>= bi.total_skip;
                sign_mask64 >>= bi.total_skip;
            }

        } // for

        // process the tail
        scalar::parse_signed(data, string + size - data, separators, output);
    }

} // namespace sse
