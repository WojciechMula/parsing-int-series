#include <vector>
#include <iterator>
#include <cstdio>
#include <cstdlib>

#include "sse/sse-matcher.h"
#include "sse/sse-parser-signed.h"

#include "application.h"

class App: public Application {

    using Vector = std::vector<uint32_t>;

public:
    App(int argc, char** argv) : Application(argc, argv) {}
    
private:
    virtual bool custom_run() override;
    virtual void custom_init() override;
};

void App::custom_init() {
    quiet = true;
}

bool App::custom_run() {
    const auto tmp = generate_signed();

    const char* separators = ",; ";
    sse::NaiveMatcher<8> matcher(separators);
    std::vector<int32_t> result;
    sse::parser_signed(tmp.data(), tmp.size(), separators, std::move(matcher), std::back_inserter(result));
    sse::stats.span_mask_histogram_to_csv(stdout);

    return true;
}

int main(int argc, char* argv[]) {

    try {
        App app(argc, argv);

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

