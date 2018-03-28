#include <vector>
#include <iterator>
#include <numeric>
#include <cstdio>
#include <cstdlib>

#include "time_utils.h"
#include "scalar/scalar-parse-unsigned.h"
#include "hybrid-parser.h"
#include "sse/sse-matcher.h"
#include "sse/sse-parser-unsigned.h"
#include "sse/sse-parser-unsigned-unrolled.h"

#include "application.h"

class BenchmarkApp: public Application {

    using Vector = std::vector<uint32_t>;

public:
    BenchmarkApp(int argc, char** argv) : Application(argc, argv) {}

public:
    bool run();

private:
    std::string tmp;
    Vector reference;
    Vector resultScalar2;
    Vector resultSSE;
    Vector resultSSEblock;

private:
    uint64_t sum(const Vector& vec) const {
        return std::accumulate(vec.begin(), vec.end(), 0);
    }
};

bool BenchmarkApp::run() {

    printf("Input size: %lu, loops: %lu\n", get_size(), get_loop_count());

    tmp = generate_unsigned();

    const char* separators = ";, ";

    const auto t0 = measure_time("scalar      : ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            reference.clear();
            scalar::parse_unsigned(tmp.data(), tmp.size(), separators,
                                   std::back_inserter(reference));
        }
    });

    const auto t1 = measure_time("hybrid      : ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            resultScalar2.clear();
            sse::NaiveMatcher<8> matcher(separators);
            hybrid_parser(tmp.data(), tmp.size(), separators,
                          std::move(matcher), std::back_inserter(resultScalar2));
        }
    });

    const auto t2 = measure_time("SSE         : ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            resultSSE.clear();
            sse::NaiveMatcher<8> matcher(separators);
            sse::parser(tmp.data(), tmp.size(), separators,
                        std::move(matcher), std::back_inserter(resultSSE));
        }
    });

    const auto t3 = measure_time("SSE (block) : ", [this, separators] {
        auto k = get_loop_count();
        while (k--) {
            resultSSEblock.clear();
            sse::NaiveMatcher<8> matcher(separators);
            sse::parser_block(tmp.data(), tmp.size(), separators,
                              std::move(matcher), std::back_inserter(resultSSEblock));
        }
    });

    puts("");
    printf("hybrid      speed-up: %0.2f\n", t0 / double(t1));
    printf("SSE         speed-up: %0.2f\n", t0 / double(t2));
    printf("SSE (block) speed-up: %0.2f\n", t0 / double(t3));

    const auto s0 = sum(reference);
    const auto s1 = sum(resultScalar2);
    const auto s2 = sum(resultSSE);
    const auto s3 = sum(resultSSEblock);
    printf("reference results: %lu %lu %lu %lu\n", s0, s1, s2, s3);

    if (s0 == s1 && s0 == s2 && s0 == s3) {
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

