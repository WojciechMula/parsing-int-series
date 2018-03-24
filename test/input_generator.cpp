#include "input_generator.h"

#include <cassert>

static const std::string numbers    = "0123456789";
static const std::string separators = ",; ";

static
std::string random_string(size_t n, const std::string& set) {
    
    std::string result(n, ' ');
    for (size_t i=0; i < n; i++) {
        result[i] = set[rand() % set.size()];
    }

    return result;
}

static
std::string generate(size_t size, size_t longest_number) {

    std::string result;

    while (true) {
        const size_t n = std::max(size_t(1), rand() % longest_number);
        const size_t k = 1;

        const std::string number = random_string(n, numbers);

        if (result.size() + n + k < size) {
            result += number;
            result += separators[0];
        } else {
            result += random_string(size - result.size(), separators);
            return result;
        }
    }
}

std::string generate(size_t size, size_t longest_number, size_t longest_separator) {

    assert(longest_number > 0);
    assert(longest_separator > 0);

    if (longest_separator == 1) {
        return generate(size, longest_number);
    }

    std::string result;

    while (true) {
        const size_t n = std::max(size_t(1), rand() % longest_number);
        const size_t k = std::max(size_t(1), rand() % longest_separator);

        const std::string number = random_string(n, numbers);
        const std::string sep    = random_string(k, separators);

        if (result.size() + n + k < size) {
            result += number;
            result += sep;
        } else {
            result += random_string(size - result.size(), separators);
            return result;
        }
    }
}

std::string generate(size_t size,
                     std::mt19937 random,
                     std::discrete_distribution<> num,
                     std::discrete_distribution<> sep) {

    std::string result;

    while (true) {
        const size_t n = num(random) + 1;
        const size_t k = sep(random) + 1;

        const std::string number = random_string(n, numbers);
        const std::string sep    = random_string(k, separators);

        if (result.size() + n + k < size) {
            result += number;
            result += sep;
        } else {
            result += random_string(size - result.size(), separators);
            return result;
        }
    }
}
