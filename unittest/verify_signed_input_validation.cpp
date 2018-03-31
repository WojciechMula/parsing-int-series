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

            const auto expected = is_valid();
            const auto result   = SSE_validate_algorithm();

            if (expected != result) {
                printf("failed for %ld: %s\n", id, input_string);
                printf("expected=%d, returned=%d\n", expected, result);
                return false;
            }
            id += 1;
        } while (!increment());

        return true;
    }

    bool SSE_validate_algorithm() {
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
            return true;
        } catch (...) {
            return false;
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

};


int main() {
    Test test;

    return test.run() ? EXIT_SUCCESS : EXIT_FAILURE;
}
