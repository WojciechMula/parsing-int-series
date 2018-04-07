#include <vector>
#include <iterator>
#include <numeric>
#include <cstdio>
#include <cstdlib>

#include "benchmark.h"
#include "scalar/std-parser-signed.h"
#include "sse/sse-matcher.h"
#include "sse/sse-parser-signed.h"
#include "sse/sse-parser-signed-unrolled.h"
#include "sse/sse-simplified-parser-signed.h"

#include "application.h"

class BenchmarkApp: public Application {

    using SignedVector = std::vector<int32_t>;

public:
    BenchmarkApp(int argc, char** argv) : Application(argc, argv) {}

public:
    bool run();

private:
    std::string tmp;

    struct ResultSigned {
        SignedVector reference;
        SignedVector SSE;
        SignedVector SSEblock;
        SignedVector std_scalar;
        SignedVector SSEsimplified;
    } result_signed;
};

bool BenchmarkApp::run() {

    printf("Input size: %lu, loops: %lu\n", get_size(), get_loop_count());

    tmp = generate_signed();

    const char* separators = ";, ";

    const auto repeat = get_loop_count();
    const auto size   = tmp.size();

    BEST_TIME(
        // pre:
        result_signed.reference.clear(),

        // test:
        scalar::parse_signed(tmp.data(), tmp.size(), separators,
                             std::back_inserter(result_signed.reference)),
        "scalar",
        repeat,
        size
    );

    BEST_TIME(
        // pre:
        result_signed.SSE.clear();
        sse::NaiveMatcher<8> matcher(separators);,

        // test:
        sse::parser_signed(tmp.data(), tmp.size(), separators,
                           std::move(matcher), std::back_inserter(result_signed.SSE)),
        "SSE",
        repeat,
        size
    );

    BEST_TIME(
        // pre:
        result_signed.SSEblock.clear();
        sse::NaiveMatcher<8> matcher(separators);,

        // test:
        sse::parser_signed_unrolled(
            tmp.data(), tmp.size(),
            separators,
            std::move(matcher), std::back_inserter(result_signed.SSEblock));,

        "SSE (block)",
        repeat,
        size
    );

    BEST_TIME(
        // pre:
        result_signed.std_scalar.clear();,

        // test:
        scalar::cstd::parse_signed(
            tmp.data(), tmp.size(),
            separators,
            std::back_inserter(result_signed.std_scalar));,

        "scalar (std)",
        repeat,
        size
    );

    BEST_TIME(
        // pre:
        result_signed.SSEsimplified.clear();,

        // result:
        sse_simplified::parse_signed(
            tmp.data(), tmp.size(),
            separators,
            std::back_inserter(result_signed.SSEsimplified));,

        "SSE (simplified)",
        repeat,
        size
    );

    return true;
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

