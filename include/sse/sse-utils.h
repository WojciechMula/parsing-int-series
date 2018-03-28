#pragma once

namespace sse {

    __m128i decimal_digits_mask(const __m128i input) {
        const __m128i ascii0 = _mm_set1_epi8('0');
        const __m128i ascii9 = _mm_set1_epi8('9' + 1);

        const __m128i t0 = _mm_cmplt_epi8(input, ascii0); // t1 = (x < '0')
        const __m128i t1 = _mm_cmplt_epi8(input, ascii9); // t0 = (x <= '9')

        return _mm_andnot_si128(t0, t1); // x <= '9' and x >= '0'
    }

}
