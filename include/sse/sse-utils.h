#pragma once

namespace sse {

    __m128i decimal_digits_mask(const __m128i input) {
        const __m128i ascii0 = _mm_set1_epi8('0');
        const __m128i ascii9 = _mm_set1_epi8('9' + 1);

        const __m128i t0 = _mm_cmplt_epi8(input, ascii0); // t1 = (x < '0')
        const __m128i t1 = _mm_cmplt_epi8(input, ascii9); // t0 = (x <= '9')

        return _mm_andnot_si128(t0, t1); // x <= '9' and x >= '0'
    }

    __m128i from_epu16(const uint16_t x, uint8_t one = 0xff) {

        uint8_t tmp[16];
        memset(tmp, 0, sizeof(tmp));

        int idx = 0;
        uint16_t val = x;
        while (val) {
            if (val & 0x0001) {
                tmp[idx] = one;
            }

            val >>= 1;
            idx += 1;
        }

        return _mm_loadu_si128((const __m128i*)tmp);
    }

}
