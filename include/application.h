#pragma once

#include <cstddef>
#include <map>
#include <string>

#include "command_line.h"
#include "discrete_distribution.h"

class Application {

public:
    class Exit {};
    class ArgumentError: public std::logic_error {
    public:
        ArgumentError(const std::string& msg) : std::logic_error(msg) {}
    };

private:
    CommandLine cmdline;

    size_t size;
    size_t debug_size;
    size_t loop_count;
    struct {
        discrete_distribution numbers;
        discrete_distribution separators;
        discrete_distribution sign;
    } distribution;
    bool sign_nonnull;
    std::string separators_set;

    std::random_device rd;
    std::mt19937 random;

protected:
    Application(int argc, char* argv[]);

    bool has_signed_distribution() const {
        return sign_nonnull;
    }

    std::string get_separators_set() const {
        return separators_set;
    }
    std::string generate_unsigned();
    std::string generate_signed();

public:
    size_t get_size() const {
        return size;
    }

    size_t get_loop_count() const {
        return loop_count;
    }

private:
    void print_help() const;
};

