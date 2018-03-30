#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <stdexcept>

#include "block_info.h"
#include "sse/sse-utils.h"

const char PLUS     = '+';
const char MINUS    = '-';
const char SEP      = '_';
const char DIGIT    = '1';
const char INVALID  = 'x';

bool SSE_validate_algorithm(const __m128i& input) {
    const __m128i ascii_minus = _mm_set1_epi8(MINUS);
    const __m128i ascii_plus  = _mm_set1_epi8(PLUS);
    const __m128i ascii_sep   = _mm_set1_epi8(SEP); // this is done by a matcher class in a real implementation
    const __m128i ascii_digit = _mm_set1_epi8(DIGIT);

    const __m128i bytemask_digit = _mm_cmpeq_epi8(input, ascii_digit);

    const __m128i bytemask_plus  = _mm_cmpeq_epi8(input, ascii_plus);
    const __m128i bytemask_minus = _mm_cmpeq_epi8(input, ascii_minus);
    const __m128i bytemask_sign  = _mm_or_si128(bytemask_plus, bytemask_minus);
    const __m128i bytemask_span  = _mm_or_si128(bytemask_digit, bytemask_sign);

    const __m128i bytemask_sep   = _mm_cmpeq_epi8(input, ascii_sep);

    const uint16_t sign_mask     = _mm_movemask_epi8(bytemask_sign);
    const uint16_t digit_mask    = _mm_movemask_epi8(bytemask_digit);
    const uint16_t valid_mask    = _mm_movemask_epi8(_mm_or_si128(bytemask_sep, bytemask_span));

    if ((valid_mask | sign_mask) != 0xffff) {
        return false;
    }

    const uint16_t span_mask = sign_mask | digit_mask;
    const BlockInfo& bi = blocks[span_mask];
    if (sign_mask & bi.invalid_sign_mask) {
        return false;
    }

    return true;
}


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

        while (!increment()) {
            prepare();

            const auto expected = is_valid();
            const auto result   = SSE_validate_algorithm(input);

            if (expected != result) {
                printf("failed for %ld: %s\n", id, input_string);
                printf("expected=%d, returned=%d\n", expected, result);
                return false;
            }
            id += 1;
        }

        return true;
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
                    // a digit can follow anything
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
