#pragma once

#include <cstdint>
#include <cassert>
#include <cstring>

#include "immintrin.h"

namespace sse {

    class STNIMatcher {
        __m128i set;
        size_t  set_size;

    public:
        static bool can_handle(const char* s) {
            return (s != nullptr)
                && (strlen(s) > 0)
                && (strlen(s) <= 16);
        }

    public:
        STNIMatcher(const char* s) {
            assert(can_handle(s));
            
            set_size = strlen(s);
            set      = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s));
        }

    public:
        __m128i get_mask(const __m128i& input, const __m128i& initial) {

            const uint8_t mode = _SIDD_UBYTE_OPS
                               | _SIDD_CMP_EQUAL_ANY
                               | _SIDD_UNIT_MASK;

            return _mm_or_si128(initial, _mm_cmpestrm(set, set_size, input, 16, mode));
        }
    };

} // namespace sse
