#include <vector>
#include <iterator>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cctype>

#include "block_info.h"
#include "scalar/scalar-parse-unsigned.h"
#include "scalar/scalar-parse-signed.h"
#include "sse/sse-convert.h"
#include "sse/sse-matcher.h"
#include "sse/sse-parser-unsigned.h"

class Verify {

    char buffer[17];
    __m128i input;

    std::vector<int32_t> result;
    std::vector<int32_t> reference;

public:
    Verify() {
        memset(buffer, 0, sizeof(buffer));
        result.resize(16);
    }

    bool run() {

        unsigned unsupported = 0;

        const __m128i ascii_minus = _mm_set1_epi8('-');
        const __m128i ascii_plus  = _mm_set1_epi8('+');

        for (int x=0; x < 65536; x++) {
            generate_input(x);

            const BlockInfo& b = blocks[x];
            const __m128i shuffle_digits = _mm_loadu_si128((const __m128i*)b.shuffle_digits);
            const __m128i shuffle_signs  = _mm_loadu_si128((const __m128i*)b.shuffle_signs);

            const __m128i bytemask_plus  = _mm_cmpeq_epi8(input, ascii_plus);
            const __m128i bytemask_minus = _mm_cmpeq_epi8(input, ascii_minus);
            const __m128i bytemask_sign  = _mm_or_si128(bytemask_plus, bytemask_minus);

            const __m128i digits   = _mm_andnot_si128(bytemask_sign, input);
            const __m128i shuffled = _mm_shuffle_epi8(digits, shuffle_digits);
            const __m128i shuffled_signs = _mm_shuffle_epi8(input, shuffle_signs);

            const __m128i negate_mask = _mm_cmpeq_epi8(shuffled_signs, ascii_minus);

            using namespace sse;

            if (b.element_size == 1) {
                convert_1digit(shuffled, b.element_count, &result[0]);
            } else if (b.element_size == 2) {
                convert_2digits_signed(shuffled, negate_mask, b.element_count, &result[0]);
            } else if (b.element_size == 4) {
                convert_4digits_signed(shuffled, negate_mask, b.element_count, &result[0]);
            } else if (b.element_size == 8) {
                convert_8digits_signed(shuffled, negate_mask, b.element_count, &result[0]);
            } else {
                unsupported += 1;
                continue;
            }

            if (!compare(b.element_count)) {
                return false;
            }
        } // for

        printf("All OK (%d cases will never be supported by SIMD code)\n", unsupported);
        return true;
    }

private:
    void generate_input(uint16_t x) {

        int k = 1;
        for (int i=0; i < 16; i++) {
            if (x & (1 << i)) {
                buffer[i] = (k % 10) + '0';
                k += 1;
            } else {
                buffer[i] = '_';
            }
        }

        char prev = '_';
        for (int i=0; i < 16; i++) {
            if (prev == '_' && isdigit(buffer[i]) && isdigit(buffer[i + 1])) {
                prev = buffer[i];
                buffer[i] = '-';
            } else {
                prev = buffer[i];
            }
        }

        input = _mm_loadu_si128((const __m128i*)buffer);

        std::fill(result.begin(), result.end(), -1);

        reference.clear();
        scalar::parse_signed(buffer, 16, "_", std::back_inserter(reference));
    }

    bool compare(size_t n) const {
        for (size_t i=0; i < n; i++) {
            if (result[i] != reference[i]) {
                printf("mismatch at %lu: expected=%d, result=%d\n", i, reference[i], result[i]);
                printf("buffer    = %s\n", buffer);
                printf("reference = "); dump(reference, n);
                printf("result    = "); dump(result, n);
                return false;
            }
        }

        return true;
    }

    void dump(const std::vector<int32_t>& vec, size_t n) const {
        for (size_t i=0; i < n; i++) {
            if (i > 0) printf(", ");
            printf("%d", vec[i]);
        }
        putchar('\n');
    }

};


int main() {

    puts("Verify SSE conversion procedure for valid inputs");
    Verify verify;
    if (!verify.run()) {
        puts("FAILED");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

