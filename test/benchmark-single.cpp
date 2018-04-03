#include <vector>
#include <iterator>
#include <numeric>
#include <cstdio>
#include <cstdlib>

#include "time_utils.h"
#include "scalar/scalar-parse-unsigned.h"
#include "sse/sse-matcher.h"
#include "sse/sse-parser-unsigned.h"
#include "sse/sse-parser-unsigned-unrolled.h"
#include "sse/sse-parser-signed.h"
#include "sse/sse-parser-signed-unrolled.h"

#include "application.h"

class BenchmarkApp: public Application {

    using Vector = std::vector<int32_t>;

    enum class Procedure {
        Scalar,
        SSE,
        SSEUnrolled
    };

    std::string procedure_name;
    Procedure procedure;

public:
    BenchmarkApp(int argc, char** argv);

public:
    void run();

private:
    Vector result;
    std::string tmp;

private:
    template <typename T>
    uint64_t sum(const T& vec) const {
        return std::accumulate(vec.begin(), vec.end(), 0);
    }

    template <typename FUN>
    Clock::time_point::rep measure_time(FUN fun) {

        Clock::time_point::rep min = 0;
        for (size_t i=0; i < get_loop_count(); i++) {
            result.clear();
            const auto t1 = Clock::now();
            fun();
            const auto t2 = Clock::now();

            const auto dt = elapsed(t1, t2);
            if (i == 0) {
                min = dt;
            } else {
                min = std::min(dt, min);
            }
        }

        return min;
    }

};

BenchmarkApp::BenchmarkApp(int argc, char** argv) : Application(argc, argv) {

    procedure_name = cmdline.get_value("--procedure", "");
    if (procedure_name.empty()) {
        throw ArgumentError("Procedure name must not be empty");
    }

    if (procedure_name == "scalar") {
        procedure = Procedure::Scalar;
    } else if (procedure_name == "sse") {
        procedure = Procedure::SSE;
    } else if (procedure_name == "sse-unrolled") {
        procedure = Procedure::SSEUnrolled;
    } else {
        throw ArgumentError("Unknown procedure name. It must be: 'scalar', 'sse', 'sse-unrolled'");
    }
}



void BenchmarkApp::run() {

    printf("Input size: %lu, loops: %lu\n", get_size(), get_loop_count());

    tmp = generate_signed();

    Clock::time_point::rep time;

    switch (procedure) {
        case Procedure::Scalar:
            time = measure_time([this] {
                    scalar::parse_signed(tmp.data(), tmp.size(), get_separators_set().c_str(),
                                         std::back_inserter(result));
                });
            break;

        case Procedure::SSE:
            time = measure_time([this] {
                    sse::NaiveMatcher<8> matcher(get_separators_set().c_str());
                    sse::parser_signed(
                        tmp.data(),
                        tmp.size(),
                        get_separators_set().c_str(),
                        std::move(matcher),
                        std::back_inserter(result));
                });
            break;

        case Procedure::SSEUnrolled:
            time = measure_time([this] {
                    sse::NaiveMatcher<8> matcher(get_separators_set().c_str());
                    sse::parser_signed_unrolled(
                        tmp.data(),
                        tmp.size(),
                        get_separators_set().c_str(),
                        std::move(matcher),
                        std::back_inserter(result));
                });
            break;

        default:
            __builtin_unreachable();
            time = 0;
            assert(false);
            break;
    }

    printf("%s: %ld us, reference results: %lu\n", procedure_name.c_str(), time, sum(result));
}


int main(int argc, char* argv[]) {

    try {
        BenchmarkApp app(argc, argv);
        app.run();

        return EXIT_SUCCESS;

    } catch (std::exception& e) {
        printf("%s\n", e.what());
        return EXIT_FAILURE;
    } catch (Application::Exit&) {
        return EXIT_SUCCESS;
    }
}

