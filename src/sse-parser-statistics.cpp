#include "sse/sse-parser-statistics.h"

namespace {

    void print_histogram(FILE* file, const sse::Statistics& stats) {
        size_t sum = 0;
        for (const auto& item: stats.total_skip_histogram) {
            sum += item.second;
        }

        for (const auto& item: stats.total_skip_histogram) {
            const int    skip  = item.first;
            const size_t count = item.second;

            fprintf(file, "* process %2d byte(s): %5lu (%5.2f%%)\n", skip, count, 100.0*count/sum);
        }
    }

}

void sse::Statistics::print(FILE* file) const {
    fprintf(file, "SSE parser statistics\n");
    fprintf(file, "total numbers converted : %lu\n", get_all_converted());
    fprintf(file, "scalar conversions      : %lu\n", scalar_conversions);

    const double perc = 100.0*get_SSE_converted()/get_all_converted();
    fprintf(file, "total SSE converted     : %lu (%0.2f%%)\n", get_SSE_converted(), perc);
    print_histogram(file, *this);

    fprintf(file, "* 1-digit vector conversions:\n");
    fprintf(file, "  - calls:          %lu\n", digit1_calls);
    fprintf(file, "  - conversions:    %lu",   digit1_conversion);
    if (digit1_calls > 0) {
        fprintf(file, " (%0.2f conversion/call)", digit1_conversion/double(digit1_calls));
    }
    fprintf(file, "\n");

    fprintf(file, "* 2-digit vector conversions:\n");
    fprintf(file, "  - calls:          %lu\n", digit2_calls);
    fprintf(file, "  - conversions:    %lu",   digit2_conversion);
    if (digit2_calls > 0) {
        fprintf(file, " (%0.2f conversion/call)", digit2_conversion/double(digit2_calls));
    }
    fprintf(file, "\n");
    
    fprintf(file, "* 4-digit vector conversions:\n");
    fprintf(file, "  - calls:          %lu\n", digit4_calls);
    fprintf(file, "  - conversions:    %lu",   digit4_conversion);
    if (digit4_calls > 0) {
        fprintf(file, " (%0.2f conversion/call)", digit4_conversion/double(digit4_calls));
    }
    fprintf(file, "\n");
    
    fprintf(file, "* 8-digit vector conversions:\n");
    fprintf(file, "  - calls:          %lu\n", digit8_calls);
    fprintf(file, "  - conversions:    %lu",   digit8_conversion);
    if (digit8_calls > 0) {
        fprintf(file, " (%0.2f conversion/call)", digit8_conversion/double(digit8_calls));
    }
    fprintf(file, "\n");
}

void sse::Statistics::init() {
    for (int i=0; i <= 16; i++)
        total_skip_histogram[i] = 0;
}
