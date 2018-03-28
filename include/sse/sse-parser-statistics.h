#pragma once

#include <cstdio>
#include <map>

#ifdef USE_STATISTICS
#define STATS_ADD(__field__, val) sse::stats.__field__ += (val)
#define STATS_INC(__field__) STATS_ADD(__field__, 1)
#else
#define STATS_ADD(__field__, val)
#define STATS_INC(__field__)
#endif

namespace sse {

    struct SSEStatistics {
        size_t scalar_conversions = 0;

        size_t digit1_calls = 0;
        size_t digit1_converted = 0;
        size_t digit2_calls = 0;
        size_t digit2_converted = 0;
        size_t digit4_calls = 0;
        size_t digit4_converted = 0;
        size_t digit8_calls = 0;
        size_t digit8_converted = 0;

        size_t get_all_converted() const {
            return scalar_conversions
                 + get_SSE_converted();
        }

        size_t get_SSE_converted() const {
            return digit1_converted
                 + digit2_converted
                 + digit4_converted
                 + digit8_converted;
        }
    };

    struct Statistics {

        std::map<int, size_t> total_skip_histogram;

        size_t loops = 0;

        SSEStatistics unsigned_path;
        SSEStatistics signed_path;

        Statistics();

        size_t get_all_converted() const {
            return unsigned_path.get_all_converted()
                 + signed_path.get_all_converted();
        }

        size_t get_scalar_conversions() const {
            return unsigned_path.scalar_conversions
                 + signed_path.scalar_conversions;
        }

        size_t get_SSE_converted() const {
            return unsigned_path.get_SSE_converted()
                 + signed_path.get_SSE_converted();
        }

        void print(FILE* file) const;

        void print() const {
            print(stdout);
        }

        void init();
    };

    extern Statistics stats;

} // namespace sse

