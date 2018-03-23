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


#ifdef SSE_COLLECT_STATISTICS
#   undef SSE_COLLECT_STATISTICS
#   define SSE_COLLECT_STATISTICS true
#else
#   define SSE_COLLECT_STATISTICS false
#endif

    namespace detail {

        template <typename INSERTER, bool collect_statistics = SSE_COLLECT_STATISTICS>
        char* parse(uint16_t mask, const __m128i input, char* data, char* end, Statistics& stats, INSERTER output) {

            const BlockInfo& b = blocks[mask];
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

                return data;
            }

            if (collect_statistics) {
                stats.total_skip_histogram[b.total_skip] += 1;
            }

            return data + b.total_skip;
        }

    } // namespace detail

    template <typename MATCHER, typename INSERTER, bool collect_statistics = SSE_COLLECT_STATISTICS>
    Statistics parser(const char* string, size_t size, const char* separators, MATCHER matcher, INSERTER output) {

        Statistics stats;
        if (collect_statistics) {
            stats.init();
        }

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

            data = detail::parse(digit_mask, input, data, end, stats, output);

        } // for

        // process the tail
        scalar_parser(data, string + size - data, separators, output);

        return stats;
    }

    template <typename MATCHER, typename INSERTER, bool collect_statistics = SSE_COLLECT_STATISTICS>
    Statistics parser_block(const char* string, size_t size, const char* separators, MATCHER matcher, INSERTER output) {

        Statistics stats;

        char* data = const_cast<char*>(string);
        char* end  = data + size;
        while (data + 16*4 < end) {
            const __m128i  input0 = _mm_loadu_si128(reinterpret_cast<__m128i*>(data + 0*16));
            const __m128i  input1 = _mm_loadu_si128(reinterpret_cast<__m128i*>(data + 1*16));
            const __m128i  input2 = _mm_loadu_si128(reinterpret_cast<__m128i*>(data + 2*16));
            const __m128i  input3 = _mm_loadu_si128(reinterpret_cast<__m128i*>(data + 3*16));
            const __m128i  t0 = decimal_digits_mask(input0);
            const uint64_t digit_mask0 = _mm_movemask_epi8(t0);
            const uint64_t sep_mask0   = _mm_movemask_epi8(matcher.get_mask(input0, t0));
            const __m128i  t1 = decimal_digits_mask(input1);
            const uint64_t digit_mask1 = _mm_movemask_epi8(t1);
            const uint64_t sep_mask1   = _mm_movemask_epi8(matcher.get_mask(input1, t1));
            const __m128i  t2 = decimal_digits_mask(input2);
            const uint64_t digit_mask2 = _mm_movemask_epi8(t2);
            const uint64_t sep_mask2   = _mm_movemask_epi8(matcher.get_mask(input2, t2));
            const __m128i  t3 = decimal_digits_mask(input3);
            const uint64_t digit_mask3 = _mm_movemask_epi8(t3);
            const uint64_t sep_mask3   = _mm_movemask_epi8(matcher.get_mask(input3, t3));

            const uint64_t digit_mask = digit_mask0
                                     | (digit_mask1 << (1*16))
                                     | (digit_mask2 << (2*16))
                                     | (digit_mask3 << (3*16));

            const uint64_t sep_mask = sep_mask0
                                    | (sep_mask1 << (1*16))
                                    | (sep_mask2 << (2*16))
                                    | (sep_mask3 << (3*16));

            if (collect_statistics) stats.loops += 1;

            if (digit_mask == 0) {
                data += 16;
                continue;
            }

            if ((digit_mask | sep_mask) != uint64_t(-1)) {
                throw std::runtime_error("Wrong character");
            }

            __m128i input = input0;
            uint64_t mask = digit_mask;
            char* loopend = data + 3*16;
            while (data < loopend) {
                char* prevdata = data;
                data = detail::parse(mask & 0xffff, input, data, end, stats, output);
                if (data == end) {
                    break;
                }

                const int shift = data - prevdata;
                mask >>= shift;
                input = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));
            }

        } // for

        // process the tail
        scalar_parser(data, string + size - data, separators, output);

        return stats;
    }

} // namespace sse
