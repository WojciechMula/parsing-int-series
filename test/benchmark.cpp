#include <vector>
#include <iterator>
#include <numeric>
#include <cstdio>
#include <cstdlib>

#include "time_utils.h"
#include "scalar-parser.h"
#include "hybrid-parser.h"
#include "sse-matcher.h"
#include "sse-parser.h"

#include "application.h"

class BenchmarkApp: public Application {

    using Vector = std::vector<uint32_t>;

public:
    BenchmarkApp(int argc, char** argv) : Application(argc, argv) {}

public:
    bool run();

    uint64_t sum(const Vector& vec) const {
        return std::accumulate(vec.begin(), vec.end(), 0);
    }
};

bool BenchmarkApp::run() {

    const std::string tmp = generate();

    Vector reference;
    Vector resultScalar2;
    Vector resultSSE;
    Vector resultSSEblock;

    const char* separators = ";, ";

    const auto t0 = measure_time("scalar      : ", [&tmp, &reference, separators] {
        scalar_parser(tmp.data(), tmp.size(), separators, std::back_inserter(reference));
    });

    const auto t1 = measure_time("hybrid      : ", [&tmp, &resultScalar2, separators] {
        sse::NaiveMatcher<8> matcher(separators);
        hybrid_parser(tmp.data(), tmp.size(), separators, std::move(matcher), std::back_inserter(resultScalar2));
    });

    const auto t2 = measure_time("SSE         : ", [&tmp, &resultSSE, separators] {
        sse::NaiveMatcher<8> matcher(separators);
        sse::parser(tmp.data(), tmp.size(), separators, std::move(matcher), std::back_inserter(resultSSE));
    });

    const auto t3 = measure_time("SSE (block) : ", [&tmp, &resultSSEblock, separators] {
        sse::NaiveMatcher<8> matcher(separators);
        sse::parser_block(tmp.data(), tmp.size(), separators, std::move(matcher), std::back_inserter(resultSSEblock));
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

