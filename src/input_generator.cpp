#include "input_generator.h"

static std::string random_string(size_t n, const std::string& set) {
    
    std::string result(n, ' ');
    for (size_t i=0; i < n; i++) {
        result[i] = set[rand() % set.size()];
    }

    return result;
}

std::string generate(size_t size, size_t longest_number, size_t longest_sep) {

    std::string result;

    static const std::string numbers    = "0123456789";
    static const std::string separators = ",; ";

    while (true) {
        const size_t n = (rand() % (longest_number - 1)) + 1;
        const size_t k = (rand() % (longest_sep - 1)) + 1;

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

