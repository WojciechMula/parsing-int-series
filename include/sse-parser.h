#pragma once

#include <vector>
#include <cassert>

#include "scalar-parser.h"
#include "sse-convert.h"
#include "block_info.h"


__m128i decimal_digits_mask(const __m128i input) {
    const __m128i ascii0 = _mm_set1_epi8('0');
    const __m128i ascii9 = _mm_set1_epi8('9' + 1);

    const __m128i t0 = _mm_cmplt_epi8(input, ascii0); // t1 = (x < '0')
    const __m128i t1 = _mm_cmplt_epi8(input, ascii9); // t0 = (x <= '9')

    return _mm_andnot_si128(t0, t1); // x <= '9' and x >= '0'
}



template <typename MATCHER, typename INSERTER>
void sse_parser(const char* string, size_t size, const char* separators, MATCHER matcher, INSERTER output) {

    char* data = const_cast<char*>(string);
    char* end  = data + size;
    while (data + 16 < end) {
        const __m128i  input = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));
        const __m128i  t0 = decimal_digits_mask(input);
        const uint16_t digit_mask = _mm_movemask_epi8(t0);
        const uint16_t sep_mask   = _mm_movemask_epi8(matcher.get_mask(input, t0));

        if (digit_mask == 0) {
            data += 16;
            continue;
        }

        if ((digit_mask | sep_mask) != 0xffff) {
            throw std::runtime_error("Wrong character");
        }


        const BlockInfo& b = blocks[digit_mask];
        const __m128i pshufb_pattern = _mm_loadu_si128((const __m128i*)b.pshufb_pattern);
        const __m128i shuffled = _mm_shuffle_epi8(input, pshufb_pattern);

        using namespace sse;

        if (b.element_size == 1) {

            convert_1digit(shuffled, b.element_count, output);

        } else if (b.element_size == 2) {

            convert_2digits(shuffled, b.element_count, output);

        } else if (b.element_size == 4) {

            convert_4digits(shuffled, b.element_count, output);

        } else if (b.element_size == 8) {

            convert_8digits(shuffled, b.element_count, output);

        } else {
            uint32_t result = 0;
            bool converted = false;
            data += b.first_skip;
            while (*data >= '0' && *data <= '9' && data < end) {
                result = result * 10 + (*data - '0');
                data += 1;
                converted = true;
            }

            if (converted) {
                *output++ = result;
            }

            continue;
        }

        data += b.total_skip;
    } // for

    // process the tail
    scalar_parser(data, string + size - data, separators, output);
}
