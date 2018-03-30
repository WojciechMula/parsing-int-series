#pragma once

#include <cstddef>
#include <string>
#include <random>

std::string generate_unsigned(size_t size,
                              const std::string& separators_set,
                              std::mt19937 random,
                              std::discrete_distribution<> numbers,
                              std::discrete_distribution<> separators);

std::string generate_signed(size_t size,
                            const std::string& separators_set,
                            std::mt19937 random,
                            std::discrete_distribution<> numbers,
                            std::discrete_distribution<> separators,
                            std::discrete_distribution<> sign);
