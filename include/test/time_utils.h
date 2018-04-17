#pragma once

#include <chrono>
#include <string>
#include <cstdio>

using Clock = std::chrono::high_resolution_clock;

template <typename UNIT = std::chrono::microseconds>
Clock::time_point::rep elapsed(const Clock::time_point& t1, const Clock::time_point& t2) {
    return std::chrono::duration_cast<UNIT>(t2 - t1).count();
}

template <typename FUN>
Clock::time_point::rep measure_time(FUN fun) {

    const auto t1 = Clock::now();
    fun();
    const auto t2 = Clock::now();

    return elapsed(t1, t2);
}

template <typename FUN>
Clock::time_point::rep measure_time(const std::string& info, FUN fun) {

    if (!info.empty()) {
        printf("%s", info.c_str());
        fflush(stdout);
    }

    const auto dt = measure_time(fun);

    if (!info.empty()) {
        printf("%lu us\n", dt);
    }

    return dt;
}

