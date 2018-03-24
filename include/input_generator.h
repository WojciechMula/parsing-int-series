#pragma once

#include <cstdint>
#include <string>
#include <random>

std::string generate(size_t size, size_t longest_number, size_t longest_sep);

std::string generate(size_t size,
                     std::mt19937 random,
                     std::discrete_distribution<> numbers,
                     std::discrete_distribution<> separators);

