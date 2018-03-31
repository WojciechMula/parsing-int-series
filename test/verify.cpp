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


class VerifyUnsignedParser {

    static const char* separators;

    static const int SIZE = 16 * 4;
    char buffer[SIZE + 1];
    std::vector<uint32_t> result;

public:
    VerifyUnsignedParser() {}

    bool run() {
        try {
            do_run();
            puts("All OK");
            return true;
        } catch (std::exception& e) {
            printf("failed: %s\n", e.what());
            dump();
            return false;
        }
    }

private:
    void do_run() {
        {
            printf("test 1... "); fflush(stdout);
            size_t cases = 0;
            for (int i=8; i <= 8; i++) {
                cases += verify1number(i);
            }
            printf(" %lu cases chcecked\n", cases);
        }

        {
            printf("test 2... "); fflush(stdout);
            size_t cases = 0;
            for (int i=1; i <= 8; i++) {
                for (int j=1; j <= 8; j++) {
                    cases += verify2numbers(i, j);
                }
            }
            printf(" %lu cases chcecked\n", cases);
        }
    }
    
private:
    size_t verify1number(const size_t digits) {
        assert(digits > 0);
        assert(digits <= 8);

        size_t cases = 0;
        uint32_t reference = test_number(digits);
        for (size_t i=0; i < SIZE; i++) {
            clear();
            if (!put_number(i, digits)) break;
            cases += 1;

            sse::NaiveMatcher<8> matcher('_');
            result.clear();
            sse::parser(buffer, SIZE, separators, matcher, std::back_inserter(result));

            if (result.size() != 1) {
                throw std::logic_error("size must be 1");
            }

            if (result[0] != reference) {
                printf("result = %u, expected = %u\n", result[0], reference);
                throw std::logic_error("wrong value");
            }
        }

        return cases;
    }

    size_t verify2numbers(int digits1, int digits2) {
        assert(digits1 > 0);
        assert(digits1 <= 8);
        assert(digits2 > 0);
        assert(digits2 <= 8);

        size_t cases = 0;

        const uint32_t reference1 = test_number(digits1);
        const uint32_t reference2 = test_number(digits2);

        for (int i = 0; i < SIZE; i++) {
            for (int j = i; j < SIZE; j++) {
                clear();
                if (!put_number(i, digits1)) continue;
                if (!put_number(j, digits2)) continue;
                cases += 1;

                sse::NaiveMatcher<8> matcher('_');
                result.clear();
                sse::parser(buffer, SIZE, separators, matcher, std::back_inserter(result));

                if (result.size() != 2) {
                    throw std::logic_error("size must be 2");
                }

                const bool e1 = (result[0] == reference1 && result[1] == reference2);
                const bool e2 = (result[0] == reference2 && result[1] == reference1);
                if (!(e1 || e2)) {
                    printf("result = %u, %u, expected = %u, %u\n", result[0], result[1], reference1, reference2);
                    throw std::logic_error("invalid value");
                }
            }
        }

        return cases;
    }

    bool put_number(int offset, int digits) {
        for (int i=0; i < digits; i++) {
            if (offset + i >= SIZE) {
                return false;
            }

            if (!is_free(offset + i)) return false;
        }

        if (!is_free(offset - 1)) return false;
        if (!is_free(offset + digits)) return false;

        for (int i=0; i < digits; i++) {
            buffer[offset + i] = ((i + 1) % 10) + '0';
        }
        return true;
    }

    bool is_free(int index) const {
        if (index < 0) return true;
        if (index >= SIZE) return true;

        return buffer[index] == '_';
    }

    uint64_t test_number(int digits) {
        uint64_t x = 0;
        for (int i=0; i < digits; i++) {
            x = 10 * x + (i + 1) % 10;
        }

        return x;
    }

    void clear() {
        memset(buffer, '_', SIZE);
        buffer[SIZE] = 0;
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

const char* VerifyUnsignedParser::separators = "_";


int main() {

    puts("Verify SSE unsigned parser");
    VerifyUnsignedParser verify;
    if (!verify.run()) {
       return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

