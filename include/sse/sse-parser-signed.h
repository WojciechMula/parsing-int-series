#pragma once

#include <vector>
#include <cassert>

#ifdef SSE_COLLECT_STATISTICS
#   undef SSE_COLLECT_STATISTICS
#   define SSE_COLLECT_STATISTICS true
#else
#   define SSE_COLLECT_STATISTICS false
#endif


#include "scalar/scalar-parse-signed.h"
#include "sse-utils.h"
#include "sse-convert.h"
#include "sse-parser-common.h"
#include "sse-parser-statistics.h"
#include "block_info.h"

namespace sse {

    template <typename MATCHER, typename INSERTER, bool collect_statistics = SSE_COLLECT_STATISTICS>
    Statistics parser_signed(const char* string, size_t size, const char* separators, MATCHER matcher, INSERTER output) {

        Statistics stats;
        if (collect_statistics) {
            stats.init();
        }

        char* data = const_cast<char*>(string);
        char* end  = data + size;

        const __m128i ascii_minus = _mm_set1_epi8('-');
        const __m128i ascii_plus  = _mm_set1_epi8('+');

        while (data + 16 < end) {
            const __m128i input = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));
            const __m128i bytemask_digit = decimal_digits_mask(input);

            const __m128i bytemask_plus  = _mm_cmpeq_epi8(input, ascii_plus);
            const __m128i bytemask_minus = _mm_cmpeq_epi8(input, ascii_minus);
            const __m128i bytemask_sign  = _mm_or_si128(bytemask_plus, bytemask_minus);

            const __m128i bytemask_span  = _mm_or_si128(bytemask_digit, bytemask_sign);
            const uint16_t sign_mask     = _mm_movemask_epi8(bytemask_sign);
            const uint16_t digit_mask    = _mm_movemask_epi8(bytemask_digit);
            const uint16_t valid_mask    = _mm_movemask_epi8(matcher.get_mask(input, t0));

            if (valid_mask | sign_mask != 0xffff) {
                throw std::runtime_error("Wrong character");
            }

            const uint16_t span_mask = sign_mask | digit_mask;
            const BlockInfo& bi = blocks[span_mask];
            // TODO: validate bytemask_sign against precalculate mask
            /*
                if (sign_mask & bi.invalid_sign_mask) {
                    throw std::runtime_error("'+' or '-' at invalid position");
                }
            */

            if (span_mask == 0) {
                data += 16;
                continue;
            }

            if (collect_statistics) stats.loops += 1;

            if (sign_mask == 0 || bi.element_size == 1) {
                // unsigned path
                data = detail::parse_unsigned(digit_mask, input, data, end, stats, output);
            } else {
                data = detail::parse_signed(bi, input, bytemask_sign, data, end, stats, output);
            }

        } // for

        // process the tail
        scalar::parse_signed(data, string + size - data, separators, output);

        return stats;
    }

} // namespace sse
