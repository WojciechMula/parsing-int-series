#pragma once

#include <vector>
#include <cassert>

#include "scalar/scalar-parse-signed.h"
#include "sse-utils.h"
#include "sse-convert.h"
#include "sse-parser-common.h"
#include "sse-parser-statistics.h"
#include "block_info.h"

namespace sse {
    
    namespace detail {

        struct result_type {
            uint64_t span_mask;
            uint64_t sign_mask;
        };

        template <typename MATCHER>
        result_type prepare_masks(char* data, MATCHER matcher) {
            const __m128i input0 = _mm_loadu_si128(reinterpret_cast<__m128i*>(data + 0*16));
            const __m128i input1 = _mm_loadu_si128(reinterpret_cast<__m128i*>(data + 1*16));
            const __m128i input2 = _mm_loadu_si128(reinterpret_cast<__m128i*>(data + 2*16));
            const __m128i input3 = _mm_loadu_si128(reinterpret_cast<__m128i*>(data + 3*16));

            const __m128i bytemask_digit0 = decimal_digits_mask(input0);
            const __m128i bytemask_digit1 = decimal_digits_mask(input1);
            const __m128i bytemask_digit2 = decimal_digits_mask(input2);
            const __m128i bytemask_digit3 = decimal_digits_mask(input3);

            const __m128i bytemask_sign0  = sign_mask(input0);
            const __m128i bytemask_sign1  = sign_mask(input1);
            const __m128i bytemask_sign2  = sign_mask(input2);
            const __m128i bytemask_sign3  = sign_mask(input3);

            const __m128i bytemask_span0  = _mm_or_si128(bytemask_digit0, bytemask_sign0);
            const __m128i bytemask_span1  = _mm_or_si128(bytemask_digit1, bytemask_sign1);
            const __m128i bytemask_span2  = _mm_or_si128(bytemask_digit2, bytemask_sign2);
            const __m128i bytemask_span3  = _mm_or_si128(bytemask_digit3, bytemask_sign3);

            const __m128i bytemask_valid = _mm_and_si128(matcher.get_mask(input0, bytemask_span0),
                                           _mm_and_si128(matcher.get_mask(input1, bytemask_span1),
                                           _mm_and_si128(matcher.get_mask(input2, bytemask_span2),
                                                         matcher.get_mask(input3, bytemask_span3))));

            if (_mm_movemask_epi8(bytemask_valid) != 0xffff) {
                throw std::runtime_error("Wrong character");
            }

            result_type res;
            res.sign_mask = compose_bitmask(bytemask_sign0,
                                            bytemask_sign1,
                                            bytemask_sign2,
                                            bytemask_sign3);

            res.span_mask = compose_bitmask(bytemask_span0,
                                            bytemask_span1,
                                            bytemask_span2,
                                            bytemask_span3);
            return res;
        }

    } // namespace detail

    template <typename MATCHER, typename INSERTER>
    void parser_signed_unrolled(
        const char* string,
        size_t size,
        const char* separators,
        MATCHER matcher,
        INSERTER output) {


        char* data = const_cast<char*>(string);
        char* end  = data + size;

        while (data + 4*16 < end) {
            detail::result_type res = detail::prepare_masks(data, matcher);

            char* loopend = data + 3*16;
            while (data < loopend) {
                const uint16_t span_mask = res.span_mask & 0xffff;
                if (span_mask == 0) {
                    res.span_mask >>= 16;
                    res.sign_mask >>= 16;
                    data += 16;

                    continue;

                }

                const BlockInfo& bi = blocks[span_mask];
                const uint16_t sign_mask = res.sign_mask & 0xffff;
                if (sign_mask & bi.invalid_sign_mask) {
                    throw std::runtime_error("'+' or '-' at invalid position");
                }

                const __m128i input = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));
                char* prevdata = data;
                if (sign_mask == 0) {
                    data = detail::parse_unsigned(bi, input, data, end, output);
                } else {
                    data = detail::parse_signed(bi, input, data, end, output);
                }

                if (data == end) {
                    break;
                }

                const int shift = data - prevdata;
                res.span_mask >>= shift;
                res.sign_mask >>= shift;
            } // inner while
            
        } // while

        // process the tail
        scalar::parse_signed(data, string + size - data, separators, output);
    }

} // namespace sse
