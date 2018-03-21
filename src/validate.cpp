#include <vector>
#include <iterator>
#include <cstdio>
#include <cstdlib>

#include "input_generator.h"
#include "scalar-parser.h"
#include "sse-parser.h"

using Vector = std::vector<uint32_t>;

void dump(const Vector& vec) {
    printf("size = %lu: [", vec.size());

    const size_t n = vec.size();
    if (n) {
        printf("%u", vec[0]);
    }

    for (size_t i=1; i < n; i++) {
        printf(", %u", vec[i]);
    }

    printf("]\n");
}

bool compare(const Vector& expected, const Vector& result) {

    if (expected.size() != result.size()) {
        puts("different sizes");
        return false;
    }

    const size_t n = expected.size();
    for (size_t i=0; i < n; i++) {
        const auto e = expected[i];
        const auto r = result[i];

        if (e != r) {
            printf("error at #%lu: expected = %u, result = %d\n", i, e, r);
            return false;
        }
    }

    return true;
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
    const auto tmp = generate(size, longest_number, longest_separator);

    assert(tmp.size() == size);

    Vector reference;
    Vector result;
    scalar_parser(tmp.data(), tmp.size(), ";, ", std::back_inserter(reference));
    sse_parser(tmp.data(), tmp.size(), ";, ", std::back_inserter(result));

    if (!compare(reference, result)) {
        puts(tmp.c_str());
        puts("");
        dump(reference);
        puts("");
        dump(result);

        return EXIT_FAILURE;
    } else {
        puts("All OK");
        return EXIT_SUCCESS;
    }
}


void print_usage() {

    puts("verify input_size longest_number longest_separator random_seed");
    puts("");
    puts("All parameters must be greater than zero");
}
