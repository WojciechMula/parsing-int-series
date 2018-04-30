#pragma once

#include <limits>
#include <stdexcept>

template <typename T>
void mul10_add_digit(T& number, char c) {
    // number * 10 + 9 <= MAX  <=>  number <= (MAX - 9)/10
    if (number < (std::numeric_limits<T>::max() - 9) / 10) {
        // no overflow is possible, use the faster path
        number = 10*number + c - '0';
    } else {
        // check for overflow
        if (__builtin_umul_overflow(number, 10, &number)) {
            throw std::range_error("unsigned overflow (1)");
        }

        if (__builtin_uadd_overflow(number, c - '0', &number)) {
            throw std::range_error("unsigned overflow (2)");
        }
    }
}
