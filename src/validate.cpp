#include <vector>
#include <iterator>
#include <cstdio>

#include "input_generator.h"
#include "scalar-parser.h"

int main() {

    const auto tmp = generate(1024, 5, 4);

    puts(tmp.c_str());

    std::vector<uint32_t> result;
    scalar_parser(tmp.data(), tmp.size(), ";, ", std::back_inserter(result));

    printf("size = %lu, element are: [", result.size());
    if (result.size()) {
        printf("%u", result[0]);
    }
    for (size_t i=1; i < result.size(); i++) {
        printf(", %u", result[i]);
    }
    putchar('\n');
}
