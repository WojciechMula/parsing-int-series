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

std::string generate_unsigned(size_t size,
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

std::string generate_signed(size_t size,
                            std::mt19937 random,
                            std::discrete_distribution<> num,
                            std::discrete_distribution<> sep,
                            std::discrete_distribution<> sign) {

    std::string result;

    while (true) {
        const size_t n = num(random) + 1;
        const size_t k = sep(random) + 1;
        const size_t s = sign(random) % 3;

        const std::string number = random_string(n, numbers);
        const std::string sep    = random_string(k, separators);

        if (result.size() + n + k + s < size) {
            switch (s) {
                case 0:
                    break;

                case 1:
                    result += '-';
                    break;

                case 2:
                    result += '+';
                    break;
            }
            result += number;
            result += sep;
        } else {
            result += random_string(size - result.size(), separators);
            return result;
        }
    }
}
