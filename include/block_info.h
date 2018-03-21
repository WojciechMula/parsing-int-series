#pragma once

#include <cstdint>

#include <immintrin.h>

struct BlockInfo {
    uint8_t first_skip;
    uint8_t first_size;
    uint8_t total_skip;
    uint8_t element_count;
    uint8_t element_size;
    uint8_t pshufb_pattern[16];
};

extern BlockInfo blocks[65536];

