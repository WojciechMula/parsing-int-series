#include <vector>
#include <iterator>
#include <cstdio>
#include <cstdlib>

#include "scalar/scalar-parse-unsigned.h"
#include "sse/sse-matcher.h"
#include "sse/sse-parser-signed.h"
#include "sse/sse-parser-signed-unrolled.h"

#include "application.h"

class CompareApp: public Application {

    using Vector = std::vector<int32_t>;

private:
    const std::string separators;
    std::string input_string;
    Vector reference;
    Vector result;

public:
    CompareApp(int argc, char** argv)
        : Application(argc, argv)
        , separators(";, ") {}

    bool run();

private:
    void run_sse_parser() {

        sse::NaiveMatcher<8> matcher(separators.c_str());
        result.clear();
        sse::parser_signed(
            input_string.data(),
            input_string.size(),
            separators.c_str(),
            std::move(matcher),
            std::back_inserter(result));
    }

    void run_unrolled_sse_parser() {

        sse::NaiveMatcher<8> matcher(separators.c_str());
        result.clear();
        sse::parser_signed_unrolled(
            input_string.data(),
            input_string.size(),
            separators.c_str(),
            std::move(matcher),
            std::back_inserter(result));
    }

private:
    void dump(const Vector& vec) const;
    bool compare(const Vector& expected, const Vector& result) const;

};

bool CompareApp::run() {

    input_string = generate_signed();
    scalar::parse_signed(input_string.data(),
                         input_string.size(),
                         separators.c_str(),
                         std::back_inserter(reference));

    puts("Checking SSE parser");
    run_sse_parser();
    if (!compare(reference, result)) {
        puts(input_string.c_str());
        puts("");
        dump(reference);
        puts("");
        dump(result);

        return false;
    }

    puts("Checking unrolled SSE parser");
    run_unrolled_sse_parser();
    if (!compare(reference, result)) {
        puts(input_string.c_str());
        puts("");
        dump(reference);
        puts("");
        dump(result);

        return false;
    }


    puts("All OK");
    return true;
}

void CompareApp::dump(const Vector& vec) const {
    printf("size = %lu: [", vec.size());

    const size_t n = vec.size();
    if (n) {
        printf("%d", vec[0]);
    }

    for (size_t i=1; i < n; i++) {
        printf(", %d", vec[i]);
    }

    printf("]\n");
}

bool CompareApp::compare(const Vector& expected, const Vector& result) const {

    if (expected.size() != result.size()) {
        puts("different sizes");
        return false;
    }

    const size_t n = expected.size();
    for (size_t i=0; i < n; i++) {
        const auto e = expected[i];
        const auto r = result[i];

        if (e != r) {
            printf("error at #%lu: expected = %d, result = %d\n", i, e, r);
            return false;
        }
    }

    return true;
}

int main(int argc, char* argv[]) {

    try {
        CompareApp app(argc, argv);

        return app.run() ? EXIT_SUCCESS : EXIT_FAILURE;

    } catch (std::exception& e) {
        printf("%s\n", e.what());
        return EXIT_FAILURE;
    } catch (Application::Exit&) {
        return EXIT_SUCCESS;
    }
}

