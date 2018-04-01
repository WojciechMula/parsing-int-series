#pragma once

#include <cstdint>
#include <cstdio>

#include <immintrin.h>

enum class Conversion: uint8_t {
    Empty,
    SSE1Digit,
    SSE2Digits,
    SSE3Digits,
    SSE4Digits,
    SSE8Digits,
    Scalar
};

struct BlockInfo {
    uint8_t     first_skip;
    uint8_t     total_skip;
    uint8_t     element_count;
    Conversion  conversion;
    uint16_t    invalid_sign_mask;
    uint8_t     shuffle_digits[16];
    uint8_t     shuffle_signs[16];

    void dump(FILE* file) const;
};

extern BlockInfo blocks[65536];

