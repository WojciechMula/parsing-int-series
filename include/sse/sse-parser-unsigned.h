#pragma once

#include <vector>
#include <cassert>

#include "scalar/scalar-parse-unsigned.h"
#include "sse-utils.h"
#include "sse-convert.h"
#include "sse-parser-common.h"
#include "sse-parser-statistics.h"
#include "block_info.h"

namespace sse {

    template <typename MATCHER, typename INSERTER>
    void parser(const char* string, size_t size, const char* separators, MATCHER matcher, INSERTER output) {

        char* data = const_cast<char*>(string);
        char* end  = data + size;

        while (data + 16 < end) {
            const __m128i  input = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));
            const __m128i  t0 = decimal_digits_mask(input);
            const uint16_t digit_mask = _mm_movemask_epi8(t0);
            const uint16_t valid_mask = _mm_movemask_epi8(matcher.get_mask(input, t0));

            STATS_INC(loops);

            if (valid_mask != 0xffff) {
                throw std::runtime_error("Wrong character");
            }

            if (digit_mask == 0) {
                data += 16;
                continue;
            }

            const BlockInfo& bi = blocks[digit_mask];
            data = detail::parse_unsigned(bi, input, data, end, output);

        } // for

        // process the tail
        scalar::parse_unsigned(data, string + size - data, separators, output);
    }

} // namespace sse
