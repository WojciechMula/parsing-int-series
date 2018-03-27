#pragma once

#include <cstdint>
#include <immintrin.h>

namespace sse {

#define SSE_ALIGN __attribute__ ((aligned (16)))

    template <typename INSERTER>
    void convert_1digit(const __m128i& input, int count, INSERTER output) {
        const __m128i ascii0 = _mm_set1_epi8('0');

        const __m128i t0 = _mm_subs_epu8(input, ascii0);

        uint8_t tmp[16] SSE_ALIGN;

        _mm_store_si128((__m128i*)tmp, t0);
        for (int i=0; i < count; i++)
            *output++ = tmp[i];
    }

    template <typename INSERTER>
    void convert_2digits(const __m128i& input, int count, INSERTER output) {
        const __m128i ascii0   = _mm_set1_epi8('0');
        const __m128i mul_1_10 = _mm_setr_epi8(10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1);

        const __m128i t0 = _mm_subs_epu8(input, ascii0);
        const __m128i t1 = _mm_maddubs_epi16(t0, mul_1_10);

        uint16_t tmp[8] SSE_ALIGN;

        _mm_store_si128((__m128i*)tmp, t1);
        for (int i=0; i < count; i++)
            *output++ = tmp[i];
    }

    template <typename INSERTER>
    void convert_4digits(const __m128i& input, int count, INSERTER output) {

        const __m128i ascii0      = _mm_set1_epi8('0');
        const __m128i mul_1_10    = _mm_setr_epi8(10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1);
        const __m128i mul_1_100   = _mm_setr_epi16(100, 1, 100, 1, 100, 1, 100, 1); 

        // =--------------

        const __m128i t0 = _mm_subs_epu8(input, ascii0);
        const __m128i t1 = _mm_maddubs_epi16(t0, mul_1_10);
        const __m128i t2 = _mm_madd_epi16(t1, mul_1_100);

        uint32_t tmp[4] SSE_ALIGN;

        _mm_store_si128((__m128i*)tmp, t2);
        for (int i=0; i < count; i++)
            *output++ = tmp[i];
    }

    template <typename INSERTER>
    void convert_8digits(const __m128i& input, int count, INSERTER output) {

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

        uint32_t tmp[4] SSE_ALIGN;

        _mm_store_si128((__m128i*)tmp, t4);
        for (int i=0; i < count; i++)
            *output++ = tmp[i];
    }

    template <typename INSERTER>
    void convert_2digits_signed(const __m128i& input, const __m128i& negate_mask, int count, INSERTER output) {
        const __m128i ascii0   = _mm_set1_epi8('0');
        const __m128i mul_1_10 = _mm_setr_epi8(10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1);

        const __m128i t0 = _mm_subs_epu8(input, ascii0);
        const __m128i t1 = _mm_maddubs_epi16(t0, mul_1_10);

        const __m128i s0 = _mm_xor_si128(t1, negate_mask);
        const __m128i s1 = _mm_sub_epi16(s0, negate_mask);

        int16_t tmp[8] SSE_ALIGN;

        _mm_store_si128((__m128i*)tmp, s1);
        for (int i=0; i < count; i++)
            *output++ = tmp[i];
    }

    template <typename INSERTER>
    void convert_4digits_signed(const __m128i& input, const __m128i& negate_mask, int count, INSERTER output) {

        const __m128i ascii0      = _mm_set1_epi8('0');
        const __m128i mul_1_10    = _mm_setr_epi8(10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1);
        const __m128i mul_1_100   = _mm_setr_epi16(100, 1, 100, 1, 100, 1, 100, 1); 

        // =--------------

        const __m128i t0 = _mm_subs_epu8(input, ascii0);
        const __m128i t1 = _mm_maddubs_epi16(t0, mul_1_10);
        const __m128i t2 = _mm_madd_epi16(t1, mul_1_100);

        const __m128i s0 = _mm_xor_si128(t2, negate_mask);
        const __m128i s1 = _mm_sub_epi32(s0, negate_mask);

        int32_t tmp[4] SSE_ALIGN;

        _mm_store_si128((__m128i*)tmp, s1);
        for (int i=0; i < count; i++)
            *output++ = tmp[i];
    }

    template <typename INSERTER>
    void convert_8digits_signed(const __m128i& input, const __m128i& negate_mask, int count, INSERTER output) {

        const __m128i ascii0      = _mm_set1_epi8('0');
        const __m128i mul_1_10    = _mm_setr_epi8(10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1);
        const __m128i mul_1_100   = _mm_setr_epi16(100, 1, 100, 1, 100, 1, 100, 1); 
        const __m128i mul_1_10000 = _mm_setr_epi16(10000, 1, 10000, 1, 10000, 1, 10000, 1);

        // =--------------

        const __m128i t0 = _mm_subs_epu8(input, ascii0);
        const __m128i t1 = _mm_maddubs_epi16(t0, mul_1_10);
        const __m128i t2 = _mm_madd_epi16(t1, mul_1_100);

        const __m128i s0 = _mm_xor_si128(t2, negate_mask);
        const __m128i s1 = _mm_sub_epi32(s0, negate_mask);
        
        const __m128i t3 = _mm_packs_epi32(s1, s1);
        const __m128i t4 = _mm_madd_epi16(t3, mul_1_10000);

        int32_t tmp[4] SSE_ALIGN;

        _mm_store_si128((__m128i*)tmp, t4);
        for (int i=0; i < count; i++)
            *output++ = tmp[i];
    }

#undef SSE_ALIGN

}

