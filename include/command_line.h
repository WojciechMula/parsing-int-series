#pragma once

#include <string>
#include <vector>

class CommandLine {

    std::vector<std::string> args;

public:
    CommandLine(int argc, char** argv);

public:
    bool empty() const { return args.size() == 1; }

    // like "-h", "--version"
    bool has_flag(const std::string& flag) const;

    // for "--name=value" returns "value"
    std::string get_value(const std::string& option) const;
    std::string get_value(const std::string& option, const std::string& defvalue) const;
    bool has_value(const std::string& option) const;

    const std::string& get_program_name() const;
};
