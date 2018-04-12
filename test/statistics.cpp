#include <vector>
#include <iterator>
#include <cstdio>
#include <cstdlib>

#include "input_generator.h"
#include "sse/sse-matcher.h"
#include "sse/sse-parser-unsigned.h"
#include "sse/sse-parser-signed.h"

#include "application.h"

class StatisticsApp: public Application {

    using Vector = std::vector<uint32_t>;

public:
    StatisticsApp(int argc, char** argv) : Application(argc, argv) {}
    
    void run();

private:
    void run_unsigned();
    void run_signed();
};

void StatisticsApp::run() {
    if (has_signed_distribution()) {
        run_signed();
    } else {
        run_unsigned();
    }
}

void StatisticsApp::run_unsigned() {

    const auto tmp = generate_unsigned();

    const char* separators = ",; ";
    sse::NaiveMatcher<8> matcher(separators);
    std::vector<uint32_t> result;
    sse::parser(tmp.data(), tmp.size(), separators, std::move(matcher), std::back_inserter(result));
    sse::stats.print();
}

void StatisticsApp::run_signed() {

    const auto tmp = generate_signed();

    const char* separators = ",; ";
    sse::NaiveMatcher<8> matcher(separators);
    std::vector<int32_t> result;
    sse::parser_signed(tmp.data(), tmp.size(), separators, std::move(matcher), std::back_inserter(result));
    sse::stats.print();
}

int main(int argc, char* argv[]) {

    try {
        StatisticsApp app(argc, argv);

        app.run();
#ifndef USE_STATISTICS
        puts("Program was not compiled with USE_STATISTICS");
#endif
        return EXIT_SUCCESS;

    } catch (std::exception& e) {
        printf("%s\n", e.what());
        return EXIT_FAILURE;
    } catch (Application::Exit&) {
        return EXIT_SUCCESS;
    }
}

