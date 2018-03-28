#pragma once

#include <vector>
#include <cassert>

#include "sse-utils.h"
#include "sse-convert.h"
#include "sse-parser-statistics.h"
#include "block_info.h"

namespace sse {

    namespace detail {

        template <typename INSERTER, bool collect_statistics = SSE_COLLECT_STATISTICS>
        char* parse(uint16_t mask, const __m128i input, char* data, char* end, Statistics& stats, INSERTER output) {

            const BlockInfo& b = blocks[mask];
            const __m128i shuffle_digits = _mm_loadu_si128((const __m128i*)b.shuffle_digits);
            const __m128i shuffled = _mm_shuffle_epi8(input, shuffle_digits);

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

} // namespace sse
