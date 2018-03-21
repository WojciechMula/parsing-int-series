#pragma once

#include <chrono>

using Clock = std::chrono::high_resolution_clock;

template <typename UNIT = std::chrono::microseconds>
Clock::time_point::rep elapsed(const Clock::time_point& t1, const Clock::time_point& t2) {
    return std::chrono::duration_cast<UNIT>(t2 - t1).count();
}

template <typename FUN>
void measure_time(const char* info, FUN fun) {

    printf("%s", info);
    fflush(stdout);
    const auto t1 = Clock::now();
    fun();
    const auto t2 = Clock::now();

    printf("%lu us\n", elapsed(t1, t2));
}

