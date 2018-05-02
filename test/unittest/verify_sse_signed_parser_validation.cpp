#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <stdexcept>

#include "block_info.h"
#include "sse/sse-matcher.h"
#include "sse/sse-parser-signed.h"

const char PLUS     = '+';
const char MINUS    = '-';
const char SEP      = '_';
const char DIGIT    = '1';
const char INVALID  = 'x';

class Test {

    enum Class {
        Separator,
        Digit,
        Sign,
        Invalid
    };

    Class input_pattern[16];
    char input_string[17];
    __m128i input;

    enum class Result {
        NoException,
        OverflowException,
        OtherException
    };

public:
    Test() {
        for (int i=0; i < 16; i++) {
            input_pattern[i] = Separator;
        }
        
        input_string[16] = 0;
        render();
    }

public:
    bool run() {
        puts("Full validation of invalid input detection in SSE approach");
        puts("Warning: this might take a few minutes on a decent computer");
        const auto ret = validate();
        if (ret) {
            puts("OK");
        } else {
            puts("!!!FAILED!!!");
        }

        return ret;
    }

private:
    bool validate() {
        long id = 0;
        
        do {
            prepare();

            if (id % 1000000 == 0) {
                printf("%ld %s\n", id, input_string);
            }

            Result expected;
            if (is_valid()) {
                if (will_overflow())
                    expected = Result::OverflowException;
                else
                    expected = Result::NoException;
            } else {
                expected = Result::OtherException;
            }

            const Result result = SSE_validate_algorithm();

            if (result != expected) {
                printf("failed for %ld: %s\n", id, input_string);
                return false;
            }
            id += 1;
        } while (!increment());

        return true;
    }

    Result SSE_validate_algorithm() {
        std::vector<int32_t> sink;
        try {
            sse::NaiveMatcher<1> matcher(SEP);
            sse::detail::process_chunk(
                input_string,
                input_string + 16,
                input,
                matcher,
                std::back_inserter(sink)
            );
            return Result::NoException;
        } catch (std::range_error&) {
            return Result::OverflowException;
        } catch (std::exception&) {
            return Result::OtherException;
        }
    }

    void prepare() {
        render();
        input = _mm_loadu_si128(reinterpret_cast<const __m128i*>(input_string));
    }

    void render() {
        int j = 0;
        for (int i=0; i < 16; i++) {
            switch (input_pattern[i]) {
                case Separator:
                    input_string[i] = SEP;
                    break;

                case Digit:
                    input_string[i] = DIGIT;
                    break;

                case Sign:
                    input_string[i] = (j++ % 2) ? PLUS : MINUS;
                    break;

                case Invalid:
                    input_string[i] = INVALID;
                    break;
            }
        }
    }

    Class next(Class x) {
        switch (x) {
            case Separator:
                return Digit;

            case Digit:
                return Sign;

            case Sign:
                return Invalid;

            case Invalid:
                return Separator;

            default:
                assert(false);
                return Separator;
        }
    }

    bool increment() {
        int i = 0;
        do {
            const auto n = next(input_pattern[i]);
            input_pattern[i] = n;
            if (n != Separator) {
                return false;
            }

            i += 1;

        } while (i < 16);

        return true;
    }


    bool is_valid() {
        Class prev = Separator;
        for (int i=0; i < 16; i++) {
            switch (input_pattern[i]) {
                case Separator:
                    if (prev == Sign) { // a solitary sign, like "__+_"
                        return false;
                    }
                    break;

                case Digit:
                    // a digit can follow anything
                    break;

                case Sign:
                    if (prev != Separator) {
                        return false;
                    }
                    break;

                case Invalid:
                    return false;
            } // switch
            prev = input_pattern[i];
        }

        return true;
    }

    uint16_t get_span_pattern() const {
        // assume is_valid() == true
        uint16_t result = 0;
        uint16_t bit = 1;
        for (int i=0; i < 16; i++, bit <<= 1) {
            switch (input_pattern[i]) {
                case Separator:
                    break;

                case Digit:
                case Sign:
                    result |= bit;
                    break;

                default:
                    assert(false);
                    return 0;

            } // switch
        }

        return result;
    }

    bool will_overflow() const {
        // assume is_valid() == true
        const BlockInfo& b = blocks[get_span_pattern()];
        if (b.conversion_routine != Conversion::Scalar) {
            // only scalar code might cause overflow error
            return false;
        }

        uint32_t result = 0;
        bool negative = false;
        for (int i=b.first_skip; i < 16; i++) {
            switch (input_pattern[i]) {
                case Separator:
                    result = 0;
                    negative = false;
                    // scalar code converts just the first span
                    return false;

                case Digit:
                    try {
                        mul10_add_digit(result, DIGIT);
                    } catch (std::range_error& e) {
                        return true;
                    }
                    break;

                case Sign:
                    negative = true; // (MINUS == ((j++ % 2) ? PLUS : MINUS));
                    result = 0;
                    break;

                default:
                    assert(false);
            } // switch
        }

        return false;
    }

};


int main() {
    Test test;

    return test.run() ? EXIT_SUCCESS : EXIT_FAILURE;
}
