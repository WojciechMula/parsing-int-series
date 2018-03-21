#include <vector>
#include <iterator>
#include <numeric>
#include <cstdio>
#include <cstdlib>

#include "input_generator.h"
#include "time_utils.h"
#include "scalar-parser.h"
#include "sse-parser.h"

using Vector = std::vector<uint32_t>;

uint64_t sum(const Vector& vec) {
    return std::accumulate(vec.begin(), vec.end(), 0);
}

void print_usage();

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
    std::string tmp;
    measure_time("generating random data ", [&tmp, size, longest_number, longest_separator]{
        tmp = generate(size, longest_number, longest_separator);
    });

    assert(tmp.size() == size);

    Vector reference;
    Vector result;

    const auto t0 = measure_time("scalar : ", [&tmp, &reference] {
        scalar_parser(tmp.data(), tmp.size(), ";, ", std::back_inserter(reference));
    });

    const auto t1 = measure_time("SSE    : ", [&tmp, &result] {
        sse_parser(tmp.data(), tmp.size(), ";, ", std::back_inserter(result));
    });

    printf("speed up: %0.2f\n", t0 / double(t1));

    const auto s1 = sum(reference);
    const auto s2 = sum(result);
    printf("reference results: %lu %lu\n", s1, s2);

    if (s1 == s2) {
        return EXIT_SUCCESS;
    } else {
        puts("FAILED");
        return EXIT_FAILURE;
    }
}


void print_usage() {

    puts("verify input_size longest_number longest_separator random_seed");
    puts("");
    puts("All parameters must be greater than zero");
}
