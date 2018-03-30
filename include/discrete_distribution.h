#pragma once

#include <random>
#include <vector>

class discrete_distribution {

public:
    using distribution_source_type = std::vector<long>;

private:
    distribution_source_type     source;
    std::discrete_distribution<> distribution;

public:
    discrete_distribution(distribution_source_type source_)
        : source(source_)
        , distribution(source.begin(), source.end()) {}

    discrete_distribution() = default;
    discrete_distribution(discrete_distribution&&) = default;
    discrete_distribution& operator=(discrete_distribution&&) = default;

public:
    std::discrete_distribution<>& get_distribution() {
        return distribution;
    }

public:
    void dump(FILE* f, int graph_width = 50) const;
};
