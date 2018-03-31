#pragma once

#include <cstdint>
#include <cstdio>

#include <immintrin.h>

struct BlockInfo {
    uint8_t  first_skip;
    uint8_t  total_skip;
    uint8_t  element_count;
    uint8_t  element_size;
    uint16_t invalid_sign_mask;
    uint8_t  shuffle_digits[16];
    uint8_t  shuffle_signs[16];

    void dump(FILE* file) const;
};

extern BlockInfo blocks[65536];

