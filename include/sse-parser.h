#pragma once

#include <cassert>

#include "sse-convert.h"
#include "block_info.h"


__m128i decimal_digits_mask(const __m128i input) {
    const __m128i ascii0 = _mm_set1_epi8('0');
    const __m128i ascii9 = _mm_set1_epi8('9' + 1);

    const __m128i t0 = _mm_cmplt_epi8(input, ascii0); // t1 = (x < '0')
    const __m128i t1 = _mm_cmplt_epi8(input, ascii9); // t0 = (x <= '9')

    return _mm_andnot_si128(t0, t1); // x <= '9' and x >= '0'
}


template <typename INSERTER>
void sse_parser(const char* string, size_t size, const char* separators, INSERTER output) {

    const size_t sse_output_size = 8; // max(blocks.element_count)
    uint32_t buffer[sse_output_size];

    char* data = const_cast<char*>(string);
    char* end  = data + size;
    while (data < end) {
        const __m128i  input = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));
        const uint16_t mask = _mm_movemask_epi8(decimal_digits_mask(input));

        if (mask == 0) {
            data += 16;
            continue;
        }

        const BlockInfo& b = blocks[mask];
        const __m128i pshufb_pattern = _mm_loadu_si128((const __m128i*)b.pshufb_pattern);
        const __m128i shuffled = _mm_shuffle_epi8(input, pshufb_pattern);

        if (b.element_size == 1) {

            convert_1digit(shuffled, b.element_count, buffer);

        } else if (b.element_size == 2) {

            convert_2digits(shuffled, b.element_count, buffer);

        } else if (b.element_size == 4) {

            convert_4digits(shuffled, b.element_count, buffer);

        } else if (b.element_size == 8) {

            convert_8digits(shuffled, b.element_count, buffer);

        } else {
            printf("unsupported: %x\n", mask);
            assert(false);
        }

        for (size_t i=0; i < b.element_count; i++) {
            *output++ = buffer[i];
        }

        data += b.total_skip;
    } // for
}
