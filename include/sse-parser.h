#pragma once

#include <vector>
#include <cassert>

#include "scalar-parser.h"
#include "sse-convert.h"
#include "sse-parser-statistics.h"
#include "block_info.h"

namespace sse {

    __m128i decimal_digits_mask(const __m128i input) {
        const __m128i ascii0 = _mm_set1_epi8('0');
        const __m128i ascii9 = _mm_set1_epi8('9' + 1);

        const __m128i t0 = _mm_cmplt_epi8(input, ascii0); // t1 = (x < '0')
        const __m128i t1 = _mm_cmplt_epi8(input, ascii9); // t0 = (x <= '9')

        return _mm_andnot_si128(t0, t1); // x <= '9' and x >= '0'
    }


} // namespace sse

#ifdef SSE_COLLECT_STATISTICS
#   undef SSE_COLLECT_STATISTICS
#   define SSE_COLLECT_STATISTICS true
#else
#   define SSE_COLLECT_STATISTICS false
#endif

template <typename MATCHER, typename INSERTER, bool collect_statistics = SSE_COLLECT_STATISTICS>
sse::Statistics sse_parser(const char* string, size_t size, const char* separators, MATCHER matcher, INSERTER output) {

    using namespace sse;

    Statistics stats;

    char* data = const_cast<char*>(string);
    char* end  = data + size;
    while (data + 16 < end) {
        const __m128i  input = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));
        const __m128i  t0 = decimal_digits_mask(input);
        const uint16_t digit_mask = _mm_movemask_epi8(t0);
        const uint16_t sep_mask   = _mm_movemask_epi8(matcher.get_mask(input, t0));

        if (collect_statistics) stats.loops += 1;

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

        if (b.element_size == 1) {

            convert_1digit(shuffled, b.element_count, output);
            if (collect_statistics) {
                stats.digit1_calls += 1;
                stats.digit1_conversion += b.element_count;
            }

        } else if (b.element_size == 2) {

            convert_2digits(shuffled, b.element_count, output);
            if (collect_statistics) {
                stats.digit2_calls += 1;
                stats.digit2_conversion += b.element_count;
            }

        } else if (b.element_size == 4) {

            convert_4digits(shuffled, b.element_count, output);
            if (collect_statistics) {
                stats.digit4_calls += 1;
                stats.digit4_conversion += b.element_count;
            }

        } else if (b.element_size == 8) {

            convert_8digits(shuffled, b.element_count, output);
            if (collect_statistics) {
                stats.digit8_calls += 1;
                stats.digit8_conversion += b.element_count;
            }

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

            if (collect_statistics) {
                stats.scalar_conversions += 1;
            }

            continue;
        }

        data += b.total_skip;
    } // for

    // process the tail
    scalar_parser(data, string + size - data, separators, output);

    return stats;
}

