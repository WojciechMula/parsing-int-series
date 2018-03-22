#include <vector>
#include <iterator>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

#include "block_info.h"
#include "scalar-parser.h"
#include "sse-convert.h"
#include "sse-matcher.h"
#include "sse-parser.h"

class VerifyConverters {

    char buffer[17];
    __m128i input;

    std::vector<uint32_t> result;
    std::vector<uint32_t> reference;

public:
    VerifyConverters() {
        memset(buffer, 0, sizeof(buffer));
        result.resize(16);
    }

    bool run() {

        unsigned unsupported = 0;

        for (int x=0; x < 65536; x++) {
            generate_input(x);

            const BlockInfo& b = blocks[x];
            const __m128i pshufb_pattern = _mm_loadu_si128((const __m128i*)b.pshufb_pattern);
            const __m128i shuffled = _mm_shuffle_epi8(input, pshufb_pattern);

            using namespace sse;

            if (b.element_size == 1) {
                convert_1digit(shuffled, b.element_count, &result[0]);
            } else if (b.element_size == 2) {
                convert_2digits(shuffled, b.element_count, &result[0]);
            } else if (b.element_size == 4) {
                convert_4digits(shuffled, b.element_count, &result[0]);
            } else if (b.element_size == 8) {
                convert_8digits(shuffled, b.element_count, &result[0]);
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

        int k = 0;
        for (int i=0; i < 16; i++) {
            if (x & (1 << i)) {
                buffer[i] = (k % 10) + '0';
                k += 1;
            } else {
                buffer[i] = '_';
            }
        }

        input = _mm_loadu_si128((const __m128i*)buffer);

        std::fill(result.begin(), result.end(), -1);

        reference.clear();
        scalar_parser(buffer, 16, "_", std::back_inserter(reference));
    }

    bool compare(size_t n) const {
        for (size_t i=0; i < n; i++) {
            if (result[i] != reference[i]) {
                printf("mismatch at %lu: expected=%u, result=%u\n", i, reference[i], result[i]);
                printf("reference = "); dump(reference, n);
                printf("result =    "); dump(result, n);
                return false;
            }
        }

        return true;
    }

    void dump(const std::vector<uint32_t>& vec, size_t n) const {
        for (size_t i=0; i < n; i++) {
            if (i > 0) printf(", ");
            printf("%u", vec[i]);
        }
        putchar('\n');
    }

};


class VerifyParser {

    static const size_t SIZE = 16 * 4;
    char buffer[SIZE + 1];
    std::vector<uint32_t> result;

public:
    VerifyParser() {}

    bool run() {
        for (size_t i=1; i <= 8; i++)
            if (!verify1number(i)) {
                puts("failed (case 1)");
                dump();
                return false;
            }

        puts("All OK");
        return true;
    }
    
private:
    bool verify1number(const size_t digits) {
        assert(digits > 0);
        assert(digits <= 8);

        for (size_t i=0; i < SIZE - digits; i++) {
            clear();

            uint32_t reference = 0;
            for (size_t j=0; j < digits; j++) {
                buffer[i + j] = j + 1 + '0';
                reference = 10 * reference + (j + 1);
            }

            sse::NaiveMatcher<8> matcher('_');
            result.clear();
            sse_parser(buffer, SIZE, matcher, std::back_inserter(result));

            if (result.size() != 1) {
                return false;
            }

            if (result[0] != reference) {
                return false;
            }
        }

        return true;
    }


    void clear() {
        memset(buffer, '_', SIZE);
        buffer[SIZE - 1] = 0;
    }

    void dump() {
        puts(buffer);
    }

    void dump(const std::vector<uint32_t>& vec) {
        printf("size = %lu: [", vec.size());

        const size_t n = vec.size();
        if (n) {
            printf("%u", vec[0]);
        }

        for (size_t i=1; i < n; i++) {
            printf(", %u", vec[i]);
        }

        printf("]\n");
    }

};



int main() {

    {
        puts("Verify SSE converters");
        VerifyConverters verify;
        if (!verify.run()) {
            return EXIT_FAILURE;
        }
    }

    {
        puts("Verify SSE parser");
        VerifyParser verify;
        if (!verify.run()) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

