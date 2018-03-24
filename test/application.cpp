#include "application.h"

#include "input_generator.h"
#include "time_utils.h"

#include <cassert>

namespace {

    std::vector<long> parse_array(const std::string& str) {
        char* c;
        const char* s = str.c_str();

        std::vector<long> result;
        while (true) {
            const long tmp = strtol(s, &c, 10);
            if (*c == ',') {
                result.push_back(tmp);
                s = c + 1;
            } else if (*c == '\0') {
                if (c != s) {
                    result.push_back(tmp);
                }
                break;
            } else {
                throw std::logic_error("Invalid character '" + std::string(1, *c) + "' in string \"" + str + "\"");
            }
        }

        if (result.empty()) {
            throw std::logic_error("Expected at least one number");
        }

        return result;
    }

}

Application::Application(int argc, char* argv[])
    : cmdline(argc, argv)
    , rd()
    , random(rd())
{
    if (cmdline.empty() || cmdline.has_flag("-h") || cmdline.has_flag("--help")) {
        print_help();
        throw Application::Exit();
    }

    size = std::stol(cmdline.get_value("--size"));
    debug_size = std::stol(cmdline.get_value("--debug", "0"));
    const auto seed = std::stol(cmdline.get_value("--seed", "0"));
    random.seed(seed);

    {
        const auto val = cmdline.get_value("--num");
        const auto arr = parse_array(val);
        numbers = std::discrete_distribution<>(arr.begin(), arr.end());
    }
    {
        const auto val = cmdline.get_value("--sep", "1");
        const auto arr = parse_array(val);
        separators = std::discrete_distribution<>(arr.begin(), arr.end());
    }

}

std::string Application::generate() {

    std::string tmp;

    measure_time("generating random data ", [&tmp, this]{
        tmp = ::generate(size, random, numbers, separators);
    });
    assert(tmp.size() == size);

    if (debug_size > 0) {
        printf("first %lu bytes of the data:\n", debug_size);
        fwrite(tmp.data(), debug_size, 1, stdout);
        putchar('\n');
    }

    return tmp;
}

void Application::print_help() const {
    printf("Usage: %s [OPTIONS]\n", cmdline.get_program_name().c_str());
    puts("");
    puts("options are");
    puts("");
    puts("--size=NUMBER         input size (in bytes)");
    puts("--seed=NUMBER         seed for random number generator [default: 0]");
    puts("--num=DISTRIBUTION    distribution of lengths of numbers");
    puts("--sep=DISTRIBUTION    distribution of lengths of gaps between numbers [default: '1']");
    puts("--debug=K             prints K first bytes of generated input [default: 0]");
}
