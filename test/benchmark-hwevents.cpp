#include <vector>
#include <iterator>
#include <numeric>
#include <cstdio>
#include <cstdlib>

#include "linux-perf-events.h"
#include "sse/sse-matcher.h"
#include "sse/sse-parser-signed.h"
#include "sse/sse-simplified-parser-signed.h"

#include "application.h"

class BenchmarkApp: public Application {

    using SignedVector = std::vector<int32_t>;

public:
    BenchmarkApp(int argc, char** argv) : Application(argc, argv) {}

private:
    virtual bool custom_run() override;
    virtual void custom_init() override;

private:
    std::string tmp;
    bool csv;

    SignedVector result;   
};

void BenchmarkApp::custom_init() {
    csv = cmdline.has_flag("--csv-output");
}

bool BenchmarkApp::custom_run() {

    if (!csv) {
        printf("Input size: %lu, loops: %lu\n", get_size(), get_loop_count());
    }

    tmp = generate_signed();

    const char* separators = ";, ";
    auto k = get_loop_count();
    
    LinuxEvents<PERF_TYPE_HARDWARE> ev_branches(PERF_COUNT_HW_BRANCH_INSTRUCTIONS);
    LinuxEvents<PERF_TYPE_HARDWARE> ev_branch_misses(PERF_COUNT_HW_BRANCH_MISSES);
    LinuxEvents<PERF_TYPE_HARDWARE> ev_cache_references(PERF_COUNT_HW_CACHE_REFERENCES);
    LinuxEvents<PERF_TYPE_HARDWARE> ev_cache_misses(PERF_COUNT_HW_CACHE_MISSES);

    ev_branches.start();
    ev_branch_misses.start();
    ev_cache_references.start();
    ev_cache_misses.start();
    while (k--) {
        result.clear();
        sse::NaiveMatcher<8> matcher(separators);
        sse::parser_signed(tmp.data(), tmp.size(), separators,
                           std::move(matcher), std::back_inserter(result));
    }

    const auto branches         = ev_branches.end();
    const auto branch_misses    = ev_branch_misses.end();
    const auto cache_references = ev_cache_references.end();
    const auto cache_misses     = ev_cache_misses.end();

    if (csv) {
        printf("%lu, %lu, %lu, %lu\n", branches, branch_misses, cache_references, cache_misses);
    } else {
        printf("branches:           %lu\n", branches);
        printf("branch misses:      %lu\n", branch_misses);
        printf("branch miss ratio:  %0.2f%%\n", (100.0 * branch_misses) / branches);
        printf("cache references:   %lu\n", cache_references);
        printf("cache misses:       %lu\n", cache_misses);
        printf("cache miss ratio:   %0.2f%%\n", (100.0 * cache_misses) / cache_references);
    }

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

