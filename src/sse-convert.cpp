#include "sse-convert.h"

/*
void dump_char(__m128i x) {
    char tmp[16];
    _mm_storeu_si128((__m128i*)tmp, x);

    putchar('[');
    for (int i=0; i < 16; i++) {
        if (isprint(tmp[i]))
            putchar(tmp[i]);
        else
            putchar('.');
    }
    printf("]\n");
}
*/

void convert_1digit(const __m128i& input, int count, uint32_t* output) {
    const __m128i ascii0 = _mm_set1_epi8('0');

    const __m128i t0 = _mm_subs_epu8(input, ascii0);

    uint8_t tmp[16];

    _mm_storeu_si128((__m128i*)tmp, t0);
    for (int i=0; i < count; i++)
        output[i] = tmp[i];
}

void convert_2digits(const __m128i& input, int count, uint32_t* output) {
    const __m128i ascii0   = _mm_set1_epi8('0');
    const __m128i mul_1_10 = _mm_setr_epi8(10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1);

    const __m128i t0 = _mm_subs_epu8(input, ascii0);
    const __m128i t1 = _mm_maddubs_epi16(t0, mul_1_10);

    uint16_t tmp[8];

    _mm_storeu_si128((__m128i*)tmp, t1);
    for (int i=0; i < count; i++)
        output[i] = tmp[i];
}

void convert_4digits(const __m128i& input, int count, uint32_t* output) {

    const __m128i ascii0      = _mm_set1_epi8('0');
    const __m128i mul_1_10    = _mm_setr_epi8(10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1);
    const __m128i mul_1_100   = _mm_setr_epi16(100, 1, 100, 1, 100, 1, 100, 1); 

    // =--------------

    const __m128i t0 = _mm_subs_epu8(input, ascii0);
    const __m128i t1 = _mm_maddubs_epi16(t0, mul_1_10);
    const __m128i t2 = _mm_madd_epi16(t1, mul_1_100);
    const __m128i t3 = _mm_packus_epi32(t2, t2);

    uint16_t tmp[8];

    _mm_storeu_si128((__m128i*)tmp, t3);
    for (int i=0; i < count; i++)
        output[i] = tmp[i];
}

void convert_8digits(const __m128i& input, int count, uint32_t* output) {

    const __m128i ascii0      = _mm_set1_epi8('0');
    const __m128i mul_1_10    = _mm_setr_epi8(10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1);
    const __m128i mul_1_100   = _mm_setr_epi16(100, 1, 100, 1, 100, 1, 100, 1); 
    const __m128i mul_1_10000 = _mm_setr_epi16(10000, 1, 10000, 1, 10000, 1, 10000, 1);

    // =--------------

    const __m128i t0 = _mm_subs_epu8(input, ascii0);
    const __m128i t1 = _mm_maddubs_epi16(t0, mul_1_10);
    const __m128i t2 = _mm_madd_epi16(t1, mul_1_100);
    const __m128i t3 = _mm_packus_epi32(t2, t2);
    const __m128i t4 = _mm_madd_epi16(t3, mul_1_10000);

    uint32_t tmp[4];

    _mm_storeu_si128((__m128i*)tmp, t4);
    for (int i=0; i < count; i++)
        output[i] = tmp[i];
}


