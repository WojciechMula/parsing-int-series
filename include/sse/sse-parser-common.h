#pragma once

#include <vector>
#include <cassert>

#include "sse-utils.h"
#include "sse-convert.h"
#include "sse-parser-statistics.h"
#include "block_info.h"

namespace sse {

    namespace detail {

        template <typename INSERTER>
        char* parse_unsigned(const BlockInfo& bi, const __m128i input, char* data, char* end, INSERTER output) {

            const __m128i shuffle_digits = _mm_loadu_si128((const __m128i*)bi.shuffle_digits);
            const __m128i shuffled = _mm_shuffle_epi8(input, shuffle_digits);

            if (bi.conversion_routine == Conversion::SSE1Digit) {

                convert_1digit(shuffled, bi.element_count, output);

                STATS_INC(unsigned_path.digit1_calls);
                STATS_ADD(unsigned_path.digit1_converted, bi.element_count);

            } else if (bi.conversion_routine == Conversion::SSE2Digits) {

                convert_2digits(shuffled, bi.element_count, output);

                STATS_INC(unsigned_path.digit2_calls);
                STATS_ADD(unsigned_path.digit2_converted, bi.element_count);

            } else if (bi.conversion_routine == Conversion::SSE3Digits) {

                convert_3digits(shuffled, bi.element_count, output);

                STATS_INC(unsigned_path.digit3_calls);
                STATS_ADD(unsigned_path.digit3_converted, bi.element_count);

            } else if (bi.conversion_routine == Conversion::SSE4Digits) {

                convert_4digits(shuffled, bi.element_count, output);

                STATS_INC(unsigned_path.digit4_calls);
                STATS_ADD(unsigned_path.digit4_converted, bi.element_count);

            } else if (bi.conversion_routine == Conversion::SSE8Digits) {

                convert_8digits(shuffled, bi.element_count, output);

                STATS_INC(unsigned_path.digit8_calls);
                STATS_ADD(unsigned_path.digit8_converted, bi.element_count);

            } else {
                uint32_t result = 0;
                bool converted = false;
                data += bi.first_skip;
                while (data < end && *data >= '0' && *data <= '9') {
                    result = result * 10 + (*data - '0');
                    data += 1;
                    converted = true;
                }

                if (converted) {
                    *output++ = result;
                }

                STATS_INC(unsigned_path.scalar_conversions);

                return data;
            }

#ifdef USE_STATISTICS
            stats.total_skip_histogram[bi.total_skip] += 1;
#endif

            return data + bi.total_skip;
        }

        template <typename INSERTER>
        char* parse_signed(
            const BlockInfo& bi,
            const __m128i input,
            char* data,
            char* end,
            INSERTER output
        ) {
            const __m128i ascii_minus = _mm_set1_epi8('-');

            const __m128i shuffle_digits = _mm_loadu_si128((const __m128i*)bi.shuffle_digits);
            const __m128i shuffle_signs  = _mm_loadu_si128((const __m128i*)bi.shuffle_signs);

            // Note: there is not need to mask '+' or '-' in the input prior
            // shuffling. This is possible because ASCII codes of '+' and '-'
            // are smaller than '0' (43 < 48 && 45 < 48). These character will
            // be zeroed during subtraction of '0'.
            const __m128i shuffled       = _mm_shuffle_epi8(input, shuffle_digits);
            const __m128i shuffled_signs = _mm_shuffle_epi8(input, shuffle_signs);
            const __m128i negate_mask    = _mm_cmpeq_epi8(shuffled_signs, ascii_minus);

            if (bi.conversion_routine == Conversion::SSE1Digit) {

                convert_1digit(shuffled, bi.element_count, output);

            } else if (bi.conversion_routine == Conversion::SSE2Digits) {

                convert_2digits_signed(shuffled, negate_mask, bi.element_count, output);

                STATS_INC(signed_path.digit2_calls);
                STATS_ADD(signed_path.digit2_converted, bi.element_count);

            } else if (bi.conversion_routine == Conversion::SSE3Digits) {

                convert_3digits_signed(shuffled, negate_mask, bi.element_count, output);

                STATS_INC(signed_path.digit3_calls);
                STATS_ADD(signed_path.digit3_converted, bi.element_count);

            } else if (bi.conversion_routine == Conversion::SSE4Digits) {

                convert_4digits_signed(shuffled, negate_mask, bi.element_count, output);

                STATS_INC(signed_path.digit4_calls);
                STATS_ADD(signed_path.digit4_converted, bi.element_count);

            } else if (bi.conversion_routine == Conversion::SSE8Digits) {

                convert_8digits_signed(shuffled, negate_mask, bi.element_count, output);

                STATS_INC(signed_path.digit8_calls);
                STATS_ADD(signed_path.digit8_converted, bi.element_count);

            } else {
                bool converted = false;
                int32_t result;
                bool negative;

                data += bi.first_skip;

                if (*data == '+') {
                    data++;
                    negative = false;
                    result = 0;
                } else if (*data == '-') {
                    data++;
                    negative = true;
                    result = 0;
                } else {
                    result = *data++ - '0';
                    negative = false;
                    converted = true;
                }

                while (data < end && *data >= '0' && *data <= '9') {
                    result = result * 10 + (*data - '0');
                    data += 1;
                    converted = true;
                }

                if (converted) {
                    if (negative) {
                        *output++ = -result;
                    } else {
                        *output++ = result;
                    }
                }

                STATS_INC(signed_path.scalar_conversions);

                return data;
            }

#ifdef USE_STATISTICS
            stats.total_skip_histogram[bi.total_skip] += 1;
#endif

            return data + bi.total_skip;
        }

    } // namespace detail

} // namespace sse
