#pragma once

#include <string>
#include <vector>
#include <stdexcept>

class CommandLine {

    std::vector<std::string> args;

    class OptionNotFound: public std::logic_error {
    public:
        OptionNotFound(const std::string& s) : std::logic_error(s) {}
    };

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

    template <typename T, typename CONVERSION>
    T parse_value(const std::string& option, CONVERSION conv) {
        try {
            return conv(get_value(option));
        } catch (OptionNotFound&) {
            throw;
        } catch (std::exception& e) {
            const auto msg = "Wrong value of '" + option + "': " + std::string(e.what());
            throw std::logic_error(msg);
        }
    }

    template <typename T, typename CONVERSION>
    T parse_value(const std::string& option, CONVERSION conv, const T& defvalue) {
        try {
            return conv(get_value(option));
        } catch (OptionNotFound&) {
            return defvalue;
        } catch (std::exception& e) {
            const auto msg = "Wrong value of '" + option + "': " + std::string(e.what());
            throw std::logic_error(msg);
        }
    }

    const std::string& get_program_name() const;
};
