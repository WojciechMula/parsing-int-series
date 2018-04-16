#include <vector>
#include <iterator>
#include <numeric>
#include <cstdio>
#include <cstdlib>

#include "time_utils.h"
#include "scalar/scalar-parse-unsigned.h"
#include "scalar/std-parser-signed.h"
#include "hybrid-parser.h"
#include "hybrid-parser-signed.h"
#include "sse/sse-matcher.h"
#include "sse/sse-parser-unsigned.h"
#include "sse/sse-block-parser-unsigned.h"
#include "sse/sse-parser-signed.h"
#include "sse/sse-block-parser-signed.h"
#include "sse/sse-simplified-parser-signed.h"

#include "application.h"

class BenchmarkApp: public Application {

    using UnsignedVector = std::vector<uint32_t>;
    using SignedVector = std::vector<int32_t>;

public:
    BenchmarkApp(int argc, char** argv) : Application(argc, argv) {}

private:
    virtual bool custom_run() override {
        if (has_signed_distribution()) {
            return run_signed();
        } else {
            return run_unsigned();
        }
    }

    bool run_unsigned();
    bool run_signed();

private:
    std::string tmp;

    struct ResultUnsigned {
        UnsignedVector reference;
        UnsignedVector hybrid;
        UnsignedVector SSE;
        UnsignedVector SSEblock;
    } result_unsigned;

    struct ResultSigned {
        SignedVector reference;
        SignedVector SSE;
        SignedVector SSEblock;
        SignedVector std_scalar;
        SignedVector SSEsimplified;
        SignedVector scalar_hybrid;
    } result_signed;

private:
    template <typename T>
    uint64_t sum(const T& vec) const {
        return std::accumulate(vec.begin(), vec.end(), 0);
    }
};

bool BenchmarkApp::run_unsigned() {

    printf("Input size: %lu, loops: %lu\n", get_size(), get_loop_count());

    tmp = generate_unsigned();

    const char* separators = ";, ";

    const auto t0 = measure_time("scalar      : ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            result_unsigned.reference.clear();
            scalar::parse_unsigned(tmp.data(), tmp.size(), separators,
                                   std::back_inserter(result_unsigned.reference));
        }
    });

    const auto t1 = measure_time("hybrid      : ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            result_unsigned.hybrid.clear();
            sse::NaiveMatcher<8> matcher(separators);
            hybrid_parser(tmp.data(), tmp.size(), separators,
                          std::move(matcher), std::back_inserter(result_unsigned.hybrid));
        }
    });

    const auto t2 = measure_time("SSE         : ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            result_unsigned.SSE.clear();
            sse::NaiveMatcher<8> matcher(separators);
            sse::parser(tmp.data(), tmp.size(), separators,
                        std::move(matcher), std::back_inserter(result_unsigned.SSE));
        }
    });

    const auto t3 = measure_time("SSE (block) : ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            result_unsigned.SSEblock.clear();
            sse::NaiveMatcher<8> matcher(separators);
            sse::parser_block_unsigned(tmp.data(), tmp.size(), separators,
                                       std::move(matcher), std::back_inserter(result_unsigned.SSEblock));
        }
    });

    const auto s0 = sum(result_unsigned.reference);
    const auto s1 = sum(result_unsigned.hybrid);
    const auto s2 = sum(result_unsigned.SSE);
    const auto s3 = sum(result_unsigned.SSEblock);

    puts("");
    printf("hybrid      speed-up: %0.2f %s\n", t0 / double(t1), s0 == s1 ? "" : "FAILED");
    printf("SSE         speed-up: %0.2f %s\n", t0 / double(t2), s0 == s2 ? "" : "FAILED");
    printf("SSE (block) speed-up: %0.2f %s\n", t0 / double(t3), s0 == s3 ? "" : "FAILED");

    printf("reference results: %lu %lu %lu %lu\n", s0, s1, s2, s3);

    if (s0 == s1 && s0 == s2 && s0 == s3) {
        return true;
    } else {
        puts("FAILED");
        return false;
    }
}


bool BenchmarkApp::run_signed() {

    printf("Input size: %lu, loops: %lu\n", get_size(), get_loop_count());

    tmp = generate_signed();

    const char* separators = ";, ";

    const auto t0 = measure_time("scalar      : ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            result_signed.reference.clear();
            scalar::parse_signed(tmp.data(), tmp.size(), separators,
                                 std::back_inserter(result_signed.reference));
        }
    });

    const auto t1 = measure_time("SSE         : ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            result_signed.SSE.clear();
            sse::NaiveMatcher<8> matcher(separators);
            sse::parser_signed(tmp.data(), tmp.size(), separators,
                               std::move(matcher), std::back_inserter(result_signed.SSE));
        }
    });

    const auto t2 = measure_time("SSE (block) : ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            result_signed.SSEblock.clear();
            sse::NaiveMatcher<8> matcher(separators);
            sse::parser_block_signed(
                tmp.data(), tmp.size(),
                separators,
                std::move(matcher), std::back_inserter(result_signed.SSEblock));
        }
    });

    const auto t3 = measure_time("scalar (std): ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            result_signed.std_scalar.clear();
            scalar::cstd::parse_signed(
                tmp.data(), tmp.size(),
                separators,
                std::back_inserter(result_signed.std_scalar));
        }
    });

    const auto t4 = measure_time("SSE (simplified): ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            result_signed.SSEsimplified.clear();
            sse_simplified::parse_signed(
                tmp.data(), tmp.size(),
                separators,
                std::back_inserter(result_signed.SSEsimplified));
        }
    });

    const auto t5 = measure_time("scalar (hybrid): ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            result_signed.scalar_hybrid.clear();
            sse::NaiveMatcher<8> matcher(separators);
            parser_hybrid_signed(
                tmp.data(), tmp.size(),
                separators,
                std::move(matcher),
                std::back_inserter(result_signed.scalar_hybrid));
        }
    });

    const auto s0 = sum(result_signed.reference);
    const auto s1 = sum(result_signed.SSE);
    const auto s2 = sum(result_signed.SSEblock);
    const auto s3 = sum(result_signed.std_scalar);
    const auto s4 = sum(result_signed.SSEsimplified);
    const auto s5 = sum(result_signed.scalar_hybrid);

    puts("");
    printf("SSE              : x %0.2f %s\n", t0 / double(t1), s0 == s1 ? "" : "FAILED");
    printf("SSE (block)      : x %0.2f %s\n", t0 / double(t2), s0 == s2 ? "" : "FAILED");
    printf("scalar (std)     : x %0.2f %s\n", t0 / double(t3), s0 == s3 ? "" : "FAILED");
    printf("SSE (simplified) : x %0.2f %s\n", t0 / double(t4), s0 == s4 ? "" : "FAILED");
    printf("scalar (hybrid)  : x %0.2f %s\n", t0 / double(t5), s0 == s5 ? "" : "FAILED");
    printf("reference results: %lu %lu %lu %lu %lu %lu\n", s0, s1, s2, s3, s4, s5);

    if (s0 == s1 && s0 == s2 && s0 == s3 && s0 == s4 && s0 == s5) {
        return true;
    } else {
        puts("FAILED");
        return false;
    }
}


int main(int argc, char* argv[]) {

    try {
        BenchmarkApp app(argc, argv);

        return app.run() ? EXIT_SUCCESS : EXIT_FAILURE;

    } catch (std::exception& e) {
        printf("%s\n", e.what());
        return EXIT_FAILURE;
    } catch (Application::Exit&) {
        return EXIT_SUCCESS;
    }
}

