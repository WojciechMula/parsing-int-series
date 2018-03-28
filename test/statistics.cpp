#include <vector>
#include <iterator>
#include <cstdio>
#include <cstdlib>

#include "input_generator.h"
#include "sse-matcher.h"
#define SSE_COLLECT_STATISTICS 1
#include "sse-parser-unsigned.h"

#include "application.h"

class StatisticsApp: public Application {

    using Vector = std::vector<uint32_t>;

public:
    StatisticsApp(int argc, char** argv) : Application(argc, argv) {}
    
    void run();
};

void StatisticsApp::run() {

    const auto tmp = generate_unsigned();

    const char* separators = ",; ";
    sse::NaiveMatcher<8> matcher(separators);
    std::vector<uint32_t> result;
    const auto stats = sse::parser(tmp.data(), tmp.size(), separators, std::move(matcher), std::back_inserter(result));
    stats.print();
}

int main(int argc, char* argv[]) {

    try {
        StatisticsApp app(argc, argv);

        app.run();
        return EXIT_SUCCESS;

    } catch (std::exception& e) {
        printf("%s\n", e.what());
        return EXIT_FAILURE;
    } catch (Application::Exit&) {
        return EXIT_SUCCESS;
    }
}

