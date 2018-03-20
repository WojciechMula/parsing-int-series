#pragma once

#include <cstdint>
#include <immintrin.h>

void convert_1digit(const __m128i& input, int count, uint32_t* output);
void convert_2digits(const __m128i& input, int count, uint32_t* output);
void convert_4digits(const __m128i& input, int count, uint32_t* output);
void convert_8digits(const __m128i& input, int count, uint32_t* output);

