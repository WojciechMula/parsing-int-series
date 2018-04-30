#include <iterator>
#include <algorithm>
#include <cstdio>

#include "sse/sse-parser-signed.h"
#include "sse/sse-matcher.h"

static const char PLUS      = '+';
static const char MINUS     = '-';
static const char SEPARATOR = ' ';

class Verify {

    enum Class {
        Separator,
        Minus,
        Plus,
        Digit
    };

    Class input_pattern[16];
    char input_string[17];
    __m128i input;
    uint16_t block_id;
    bool range_error_expected;

    using Vector = std::vector<int32_t>;

    Vector input_values;
    Vector parsed_values;

public:
    Verify() {
        for (int i=0; i < 16; i++) {
            input_pattern[i] = Separator;
        }

        for (int i=0; i < 17; i++) {
            input_string[i] = 0;
        }
    }

    bool run() {
        long int id = -1;
        long int checked = 0;
        do {
            id += 1;
            if (!is_valid())
                continue;

            prepare();
            sse::NaiveMatcher<1> matcher(SEPARATOR);
            try {
                sse::detail::process_chunk(
                    input_string,
                    input_string + 16,
                    input,
                    matcher,
                    std::back_inserter(parsed_values));

                if (range_error_expected) {
                    puts("expected range error");
                    printf("input string : '%s'\n", input_string);
                    return false;
                }

                if (!compare(input_values, parsed_values)) {
                    printf("input id     : %ld\n", id);
                    printf("input string : '%s'\n", input_string);
                    printf("block id     : %02x\n", block_id);
                    printf("expected     :"); dump(input_values);
                    printf("parsed       :"); dump(parsed_values);
                    return false;
                }
            } catch (std::range_error&) {
                if (!range_error_expected) {
                    puts("unexpected range error");
                    printf("input string : '%s'\n", input_string);
                    return false;
                }
            }

            checked += 1;

        } while (increment());

        printf("All OK, checked %ld cases\n", checked);
        return true;
    }

private:
    Class next(Class x) {
        switch (x) {
            case Separator:
                return Minus;

            case Minus:
                return Plus;

            case Plus:
                return Digit;

            case Digit:
                return Separator;

            default:
                assert(false);
                return Separator;
        }
    }

    bool increment() {
        int i = 0;
        do {
            const Class n = next(input_pattern[i]);
            input_pattern[i] = n;
            if (n != Separator) {
                return true;
            }

            i += 1;
        } while (i < 16);

        return false;
    }

    bool is_valid() const {
        Class prev = Separator;
        for (int i=0; i < 16; i++) {
            switch (input_pattern[i]) {
                case Separator:
                    if (prev == Plus || prev == Minus) {
                        return false;
                    }
                    break;
                
                case Plus:
                case Minus:
                    if (prev == Plus || prev == Minus || prev == Digit) {
                        return false;
                    }   
                    break;

                case Digit:
                    if (prev == Plus || prev == Minus) {
                        return false;
                    }   
                    break;
            } // switch

            prev = input_pattern[i];
        }

        return true;
    }

    uint16_t get_block_id() const {
        uint16_t block_id = 0;

        for (int i=0; i < 16; i++) {
            switch (input_pattern[i]) {
                case Separator:
                    break;
                
                case Minus:
                case Plus:
                case Digit:
                    block_id |= (1 << i);
                    break;
            }
        }

        return block_id;
    }
    
    void prepare() {
        input_values.clear();
        parsed_values.clear();

        Class prev = Separator;
        int32_t value = 0;
        bool negative = false;
        char digit = 1;

        std::vector<bool> range_errors;
        range_error_expected = false;
        for (int i=0; i < 16; i++) {
            switch (input_pattern[i]) {
                // convert
                case Separator:
                    input_string[i] = SEPARATOR;
                    if (prev == Digit) {
                        if (negative) {
                            value = -value;
                        }

                        input_values.push_back(value);
                        range_errors.push_back(range_error_expected);
                        range_error_expected = false;
                    }
                    break;
                
                case Minus:
                    input_string[i] = MINUS;
                    value = 0;
                    negative = true;
                    digit = 1;
                    break;

                case Plus:
                    input_string[i] = PLUS;
                    value = 0;
                    negative = false;
                    digit = 1;
                    break;
                
                case Digit:
                    if (prev == Separator) {
                        value = 0;
                        negative = false;
                        digit = 1;
                    }

                    input_string[i] = digit + '0';
                    uint32_t tmp = value;
                    try {
                        mul10_add_digit(tmp, input_string[i]);
                        value = tmp;
                    } catch (std::range_error&) {
                        range_error_expected = true;
                    }
                    digit = (digit + 1) % 10;
                    break;
            } // switch

            prev = input_pattern[i];
        }


        block_id = get_block_id();
        const BlockInfo& bi = blocks[block_id];
        if (bi.element_count > 0) {
            input_values.resize(bi.element_count);
            range_errors.resize(bi.element_count);
        } else {
            if (prev == Digit) {
                if (negative) {
                    value = -value;
                }

                input_values.push_back(value);
                range_errors.push_back(range_error_expected);
            }
        }

        range_error_expected = std::any_of(range_errors.begin(), range_errors.end(), [](bool x){return x;});

        input = _mm_loadu_si128(reinterpret_cast<const __m128i*>(input_string));
    }

    bool compare(const Vector& expected, const Vector& result) const {
        if (expected.size() != result.size()) {
            puts("different size");
            return false;
        }
    
        const size_t n = expected.size();
        for (size_t i=0; i < n; i++) {
            if (expected[i] != result[i]) {
                printf("mismatched value #%lu: expected=%d, result=%d\n", i, expected[i], result[i]);
                return false;
            }
        }

        return true;
    }

    void dump(const Vector& v) const {
        putchar('[');
        const size_t n = v.size();
        if (n > 0) {
            printf("%d", v[0]);
        }

        for (size_t i=1; i < n; i++) {
            printf(", %d", v[1]);
        }
        printf("]\n");
    }

};


int main() {
    puts("Verify sse::signed_parser against all possible valid inputs");
    Verify verify;
    if (verify.run()) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
