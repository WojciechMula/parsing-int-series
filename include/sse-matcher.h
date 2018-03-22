#pragma once

#include <cassert>
#include <cstring>

#include "immintrin.h"

namespace sse {

    template<int K>
    class NaiveMatcher {
        __m128i letters[K + 1];
        size_t n;

    public:
        NaiveMatcher(const char* s) {
            assert(s != nullptr);
            n = strlen(s);
            assert(n < K);

            for (size_t i=0; i < n + 1; i++) {
                letters[i] = _mm_set1_epi8(s[i]);
            }
        }

        NaiveMatcher(char c) {
            letters[0] = _mm_set1_epi8(c);
        }

    public:
        __m128i get_mask(const __m128i& input, const __m128i& initial) const {
            __m128i result = initial;
            for (size_t i=0; i < n + 1; i++) {

                const __m128i mask = _mm_cmpeq_epi8(letters[i], input);
                result = _mm_or_si128(result, mask);
            }

            return result;
        }
    };

} // namespace sse
