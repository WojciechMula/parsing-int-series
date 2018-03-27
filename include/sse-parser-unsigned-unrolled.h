#pragma once

#include <vector>
#include <cassert>

#ifdef SSE_COLLECT_STATISTICS
#   undef SSE_COLLECT_STATISTICS
#   define SSE_COLLECT_STATISTICS true
#else
#   define SSE_COLLECT_STATISTICS false
#endif

#include "scalar/scalar-parse-unsigned.h"
#include "sse-utils.h"
#include "sse-convert.h"
#include "sse-parser-common.h"
#include "sse-parser-statistics.h"
#include "block_info.h"

namespace sse {

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
            const uint64_t valid_mask0 = _mm_movemask_epi8(matcher.get_mask(input0, t0));
            const __m128i  t1 = decimal_digits_mask(input1);
            const uint64_t digit_mask1 = _mm_movemask_epi8(t1);
            const uint64_t valid_mask1 = _mm_movemask_epi8(matcher.get_mask(input1, t1));
            const __m128i  t2 = decimal_digits_mask(input2);
            const uint64_t digit_mask2 = _mm_movemask_epi8(t2);
            const uint64_t valid_mask2 = _mm_movemask_epi8(matcher.get_mask(input2, t2));
            const __m128i  t3 = decimal_digits_mask(input3);
            const uint64_t digit_mask3 = _mm_movemask_epi8(t3);
            const uint64_t valid_mask3 = _mm_movemask_epi8(matcher.get_mask(input3, t3));

            const uint64_t valid_mask = valid_mask0
                                      | (valid_mask1 << (1*16))
                                      | (valid_mask2 << (2*16))
                                      | (valid_mask3 << (3*16));

            if (collect_statistics) stats.loops += 1;

            if (valid_mask != uint64_t(-1)) {
                throw std::runtime_error("Wrong character");
            }

            const uint64_t digit_mask = digit_mask0
                                     | (digit_mask1 << (1*16))
                                     | (digit_mask2 << (2*16))
                                     | (digit_mask3 << (3*16));

            if (digit_mask == 0) {
                data += 16*4;
                continue;
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
        scalar::parse_unsigned(data, string + size - data, separators, output);

        return stats;
    }

} // namespace sse
