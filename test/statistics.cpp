#include <vector>
#include <iterator>
#include <cstdio>
#include <cstdlib>

#include "input_generator.h"
#include "sse-matcher.h"
#define SSE_COLLECT_STATISTICS 1
#include "sse-parser.h"

using Vector = std::vector<uint32_t>;

void print_usage();
void print_statistics(const sse::Statistics& stats);

int main(int argc, char* argv[]) {

    if (argc < 5) {
        print_usage();
        return EXIT_FAILURE;
    }

    const size_t size = atoi(argv[1]);
    const size_t longest_number = atoi(argv[2]);
    const size_t longest_separator = atoi(argv[3]);
    const int seed = atoi(argv[4]);

    srand(seed);

    printf("size = %lu, longest number = %lu, longest gap = %lu\n", size, longest_number, longest_separator);
    const auto tmp = generate(size, longest_number, longest_separator);

    const char* separators = ",; ";
    sse::NaiveMatcher<8> matcher(separators);
    std::vector<uint32_t> result;
    const auto stats = sse_parser(tmp.data(), tmp.size(), separators, std::move(matcher), std::back_inserter(result));
    print_statistics(stats);

    return EXIT_SUCCESS;
}

void print_statistics(const sse::Statistics& stats) {
    printf("SSE parser statistics\n");
    printf("total numbers converted : %lu\n", stats.get_all_converted());
    printf("total SSE converted     : %lu\n", stats.get_SSE_converted());
    printf("scalar conversions      : %lu\n", stats.scalar_conversions);

    printf("* 1-digit vector conversions:\n");
    printf("  - calls:          %lu\n", stats.digit1_calls);
    printf("  - conversions:    %lu",   stats.digit1_conversion);
    if (stats.digit1_calls > 0) {
        printf(" (%0.2f conversion/call)", stats.digit1_conversion/double(stats.digit1_calls));
    }
    putchar('\n');

    printf("* 2-digit vector conversions:\n");
    printf("  - calls:          %lu\n", stats.digit2_calls);
    printf("  - conversions:    %lu",   stats.digit2_conversion);
    if (stats.digit2_calls > 0) {
        printf(" (%0.2f conversion/call)", stats.digit2_conversion/double(stats.digit2_calls));
    }
    putchar('\n');
    
    printf("* 4-digit vector conversions:\n");
    printf("  - calls:          %lu\n", stats.digit4_calls);
    printf("  - conversions:    %lu",   stats.digit4_conversion);
    if (stats.digit4_calls > 0) {
        printf(" (%0.2f conversion/call)", stats.digit4_conversion/double(stats.digit4_calls));
    }
    putchar('\n');
    
    printf("* 8-digit vector conversions:\n");
    printf("  - calls:          %lu\n", stats.digit8_calls);
    printf("  - conversions:    %lu",   stats.digit8_conversion);
    if (stats.digit8_calls > 0) {
        printf(" (%0.2f conversion/call)", stats.digit8_conversion/double(stats.digit8_calls));
    }
    putchar('\n');
}

void print_usage() {

    puts("verify input_size longest_number longest_separator random_seed");
    puts("");
    puts("All parameters must be greater than zero");
}
