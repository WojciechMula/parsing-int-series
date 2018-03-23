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
    stats.print();

    return EXIT_SUCCESS;
}

void print_usage() {

    puts("verify input_size longest_number longest_separator random_seed");
    puts("");
    puts("All parameters must be greater than zero");
}
