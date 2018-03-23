#pragma once

#include <cstdio>
#include <map>

namespace sse {

    struct Statistics {
 
        std::map<int, size_t> total_skip_histogram;

        size_t loops = 0;
        size_t scalar_conversions = 0;
        size_t digit1_calls = 0;
        size_t digit1_conversion = 0;
        size_t digit2_calls = 0;
        size_t digit2_conversion = 0;
        size_t digit4_calls = 0;
        size_t digit4_conversion = 0;
        size_t digit8_calls = 0;
        size_t digit8_conversion = 0;

        size_t get_all_converted() const {
            return get_SSE_converted() + scalar_conversions;
        }

        size_t get_SSE_converted() const {
            return digit1_conversion
                 + digit2_conversion
                 + digit4_conversion
                 + digit8_conversion;
        }

        void print(FILE* file) const;

        void print() const {
            print(stdout);
        }

        void init();
    };

} // namespace sse

