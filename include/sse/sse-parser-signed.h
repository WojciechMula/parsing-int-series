#pragma once

#include <vector>
#include <cassert>

#include "scalar/scalar-parse-signed.h"
#include "sse-utils.h"
#include "sse-convert.h"
#include "sse-parser-common.h"
#include "sse-parser-statistics.h"
#include "block_info.h"
#include "sse-dump.h"

namespace sse {

    namespace detail {

        template <typename MATCHER, typename INSERTER>
        char* process_chunk(char* data, char* end, const __m128i& input, MATCHER matcher, INSERTER output) {

            const __m128i ascii_minus = _mm_set1_epi8('-');
            const __m128i ascii_plus  = _mm_set1_epi8('+');

            const __m128i bytemask_digit = decimal_digits_mask(input);

            const __m128i bytemask_plus  = _mm_cmpeq_epi8(input, ascii_plus);
            const __m128i bytemask_minus = _mm_cmpeq_epi8(input, ascii_minus);
            const __m128i bytemask_sign  = _mm_or_si128(bytemask_plus, bytemask_minus);
            const __m128i bytemask_span  = _mm_or_si128(bytemask_digit, bytemask_sign);

            const uint16_t valid_mask    = _mm_movemask_epi8(matcher.get_mask(input, bytemask_span));

            if (valid_mask != 0xffff) {
                throw std::runtime_error("Wrong character");
            }

            const uint16_t sign_mask = _mm_movemask_epi8(bytemask_sign);
            const uint16_t span_mask = _mm_movemask_epi8(bytemask_span);
            const BlockInfo& bi      = blocks[span_mask];
            if (sign_mask & bi.invalid_sign_mask) {
                throw std::runtime_error("'+' or '-' at invalid position");
            }

            if (span_mask == 0) {
                return data + 16;
            }

            STATS_INC(loops);

            if (sign_mask == 0 || bi.element_size == 1) {
                // unsigned path
                return detail::parse_unsigned(bi, input, data, end, output);
            } else {
                return detail::parse_signed(bi, input, data, end, output);
            }
        }

    }

    template <typename MATCHER, typename INSERTER>
    void parser_signed(const char* string, size_t size, const char* separators, MATCHER matcher, INSERTER output) {

        char* data = const_cast<char*>(string);
        char* end  = data + size;

        while (data + 16 < end) {
            const __m128i input = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));
            data = detail::process_chunk(data, end, input, matcher, output);

        } // for

        // process the tail
        scalar::parse_signed(data, string + size - data, separators, output);
    }

} // namespace sse
