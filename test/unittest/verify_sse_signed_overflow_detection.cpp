#include <iterator>
#include <cstdio>
#include <limits>

#include "sse/sse-parser-signed.h"
#include "sse/sse-matcher.h"


const char SEPARATOR = '_';

class Verify {

    using Vector = std::vector<int32_t>;
    Vector result;

    std::string input;
    std::string image;
    size_t      size;
    size_t      position;
    int64_t     value;
public:

    Verify() : size(64) {}

    bool run() {

        //check_not_overflow();
        check_overflow();

        printf("All OK\n");
        return true;
    }

private:
    void check_not_overflow() {

        value = std::numeric_limits<int32_t>::max();
        image = std::to_string(value);

        printf("'%s' should not overflow\n", image.c_str());
        assume_not_overflow();

        image = '+' + image;
        printf("'%s' should not overflow\n", image.c_str());
        assume_not_overflow();

        value = std::numeric_limits<int32_t>::min();
        image = std::to_string(value);

        printf("'%s' should not overflow\n", image.c_str());
        assume_not_overflow();
    }

    void assume_not_overflow() {

        for (position=0; position < 32; position++) {

            prepare_input();
            convert();

            assert(result.size() == 1);
            assert(result[0] == value);
        }
    }

    void check_overflow() {

        value = std::numeric_limits<int32_t>::max();
        value += 1;
        image = std::to_string(value);

        printf("'%s' should overflow\n", image.c_str());
        assume_overflow();

        image = '+' + image;
        printf("'%s' should overflow\n", image.c_str());
        assume_overflow();

        value = 9999999999l;
        image = std::to_string(value);

        printf("'%s' should overflow\n", image.c_str());
        assume_overflow();

        value = 100000000000000l;
        image = std::to_string(value);

        printf("'%s' should overflow\n", image.c_str());
        assume_overflow();

        value = std::numeric_limits<int32_t>::min();
        value -= 1;
        image = std::to_string(value);

        printf("'%s' should overflow\n", image.c_str());
        assume_overflow();

        value = -9999999999l;
        image = std::to_string(value);

        printf("'%s' should overflow\n", image.c_str());
        assume_overflow();

        value = -100000000000000l;
        image = std::to_string(value);

        printf("'%s' should overflow\n", image.c_str());
        assume_overflow();
    }

    void assume_overflow() {

        for (position=0; position < 32; position++) {

            prepare_input();
            try {
                convert();
                assert(false && "must fail");
            } catch (std::range_error& e) {
                assert(result.size() == 0);
            } catch (...) {
                assert(false && "unexpected exception");
            }
        }
    }

    void prepare_input() {
        input.clear();
        input += std::string(position, SEPARATOR);
        input += image;
        input += std::string(size - input.size(), SEPARATOR);
    }

    void convert() {
        const char separators[] = {SEPARATOR, 0};

        result.clear();
        sse::NaiveMatcher<8> matcher(separators);

        sse::parser_signed(input.data(),
                           input.size(), separators,
                           std::move(matcher),
                           std::back_inserter(result));
    }
};


int main() {
    puts("Verify if sse::signed_parser detects overflows");
    Verify verify;
    if (verify.run()) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
