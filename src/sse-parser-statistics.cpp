#include "sse/sse-parser-statistics.h"
#include <vector>
#include <algorithm>
#include <cassert>

sse::Statistics sse::stats; // a global object

namespace {

    void print_skip_histogram(FILE* file, const sse::Statistics& stats) {
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

    struct span_histogram_entry {
        uint16_t mask;
        size_t   count;

        span_histogram_entry(uint16_t mask_, size_t count_)
            : mask(mask_)
            , count(count_) {}
    };

    using span_histogram = std::vector<span_histogram_entry>;

    void print_span_mask_histogram(FILE* file, const sse::Statistics& stats) {
        size_t sum = 0;
        span_histogram histogram;
        for (const auto& item: stats.span_masks_histogram) {
            sum += item.second;
            histogram.emplace_back(item.first, item.second);
        }

        std::sort(histogram.begin(), histogram.end(),
                  [](const span_histogram_entry& a, const span_histogram_entry& b){return a.count < b.count;});

        printf("Span mask histogram (%lu entries)\n", stats.span_masks_histogram.size());
        size_t cumulative = 0;
        size_t id = 0;
        for (const auto& item: histogram) {
            cumulative += item.count;
            fprintf(file, "%5lu 0x%02x: %5lu (%5.2f%%; cumulative %5.2f%%)\n",
                    id++,
                    item.mask, item.count,
                    100.0*item.count/sum,
                    100.0*cumulative/sum);
        }
    }

    void print_sse_statistics(FILE* file, const char* title, size_t calls, size_t converted) {

        fprintf(file, "* %s:", title);
        if (calls == 0 && converted == 0) {
            printf(" none\n");
            return;
        } else {
            printf("\n");
        }

        fprintf(file, "  - calls:           %8lu\n", calls);
        fprintf(file, "  - converted nums:  %8lu\n", converted);
        fprintf(file, "  - conversion/call: ");
        if (calls > 0) {
            fprintf(file, "%11.2f", converted/double(calls));
        } else {
            fprintf(file, "-");
        }
        fprintf(file, "\n");
    }

}

void sse::Statistics::print(FILE* file) const {
    fprintf(file, "SSE parser statistics\n");
    fprintf(file, "loops                   : %8lu\n", loops);
    fprintf(file, "total numbers converted : %8lu\n", get_all_converted());
    fprintf(file, "scalar conversions      : %8lu\n", get_scalar_conversions());

    const double perc_total = 100.0*get_SSE_converted()/get_all_converted();
    fprintf(file, "all converted by SSE    : %8lu (%0.2f%%)\n", get_SSE_converted(), perc_total);
    fprintf(file, " - by unsinged routines : %8lu\n", unsigned_path.get_SSE_converted());
    fprintf(file, " - by singed routines   : %8lu\n", signed_path.get_SSE_converted());
    print_skip_histogram(file, *this);

    print_sse_statistics(file, "1-digit vector conversions (unsigned)", unsigned_path.digit1_calls, unsigned_path.digit1_converted);
    print_sse_statistics(file, "2-digit vector conversions (unsigned)", unsigned_path.digit2_calls, unsigned_path.digit2_converted);
    print_sse_statistics(file, "2-digit vector conversions (signed)",   signed_path.digit2_calls,   signed_path.digit2_converted);
    print_sse_statistics(file, "3-digit vector conversions (unsigned)", unsigned_path.digit3_calls, unsigned_path.digit3_converted);
    print_sse_statistics(file, "3-digit vector conversions (signed)",   signed_path.digit3_calls,   signed_path.digit3_converted);
    print_sse_statistics(file, "4-digit vector conversions (unsigned)", unsigned_path.digit4_calls, unsigned_path.digit4_converted);
    print_sse_statistics(file, "4-digit vector conversions (signed)",   signed_path.digit4_calls,   signed_path.digit4_converted);
    print_sse_statistics(file, "8-digit vector conversions (unsigned)", unsigned_path.digit8_calls, unsigned_path.digit8_converted);
    print_sse_statistics(file, "8-digit vector conversions (signed)",   signed_path.digit8_calls,   signed_path.digit8_converted);

    print_span_mask_histogram(file, *this);
}

sse::Statistics::Statistics() {
    for (int i=0; i <= 16; i++)
        total_skip_histogram[i] = 0;
}

void sse::Statistics::span_mask_histogram_to_csv(FILE* file) const {
    assert(file != nullptr);

    for (const auto& item: stats.span_masks_histogram) {
        const uint16_t mask  = item.first;
        const size_t   count = item.second;
        fprintf(file, "%02x, %lu\n", mask, count);
    }
}
