#pragma once

#include <cstddef>
#include <map>
#include <random>
#include <string>

#include "command_line.h"

class Application {

public:
    class Exit {};
    class ArgumentError: public std::logic_error {
    public:
        ArgumentError(const std::string& msg) : std::logic_error(msg) {}
    };

protected:
    CommandLine cmdline;

protected:
    bool quiet;

private:
    size_t size;
    size_t debug_size;
    size_t loop_count;
    struct {
        std::discrete_distribution<> numbers;
        std::discrete_distribution<> separators;
        std::discrete_distribution<> sign;
    } distribution;
    bool sign_nonnull;
    std::string separators_set;

    std::random_device rd;
    std::mt19937 random;

public:
    bool run();

protected:
    Application(int argc, char* argv[]);

    virtual bool custom_run() = 0;
    virtual void custom_init();

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

protected:
    virtual void print_custom_help() const;

private:
    void init();
    void print_help() const;
};

