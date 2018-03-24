#include "command_line.h"

#include <algorithm>
#include <stdexcept>
#include <cstring>

CommandLine::CommandLine(int argc, char** argv) {
    for (int i=0; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
}

bool CommandLine::has_flag(const std::string& flag) const {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

namespace {
    
    // is s2 prefix of s1
    bool is_prefix(const std::string s1, const std::string s2) {
        if (s2.size() > s1.size()) {
            return false;
        }

        if (s1.size() == s2.size()) {
            return s1 == s2;
        }

        return memcmp(s1.c_str(), s2.c_str(), s2.size()) == 0;
    }
}

std::string CommandLine::get_value(const std::string& option) const {

    for (size_t i=0; i < args.size(); i++) {
        if (args[i] == option) {
            try {
                return args[i + 1];
            } catch (std::out_of_range&) {
                throw std::logic_error("Argument " + args[i] + " should be followed by a value");
            }
        }
    }

    const auto long_option = option + "=";
    for (const auto& arg: args) {
        if (is_prefix(arg, long_option)) {
            return arg.substr(long_option.size());
        }
    }

    throw std::logic_error("Argument " + option + " not found");
}

std::string CommandLine::get_value(const std::string& option, const std::string& defvalue) const {
    try {
        return get_value(option);
    } catch (std::logic_error&) {
        return defvalue;
    }
}

bool CommandLine::has_value(const std::string& option) const {
    try {
        get_value(option);
        return true;
    } catch (std::logic_error&) {
        return false;
    }
}

const std::string& CommandLine::get_program_name() const {
    return args[0];
}

