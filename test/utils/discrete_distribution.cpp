#include "discrete_distribution.h"

#include <numeric>
#include <cassert>

namespace {

    std::string bar(char c, long num, long den, int graph_width) {
        double perc = (den != 0) ? num/double(den) : 0.0;
        const int count = graph_width * perc;

        return std::string(c, count);
    }

}

void discrete_distribution::dump(FILE* f, int graph_width) const {
    assert(f != nullptr);

    const long sum = std::accumulate(source.begin(), source.end(), 0);
    int id = 1;
    for (const long val: source) {
       fprintf(f, "%2d: %5ld %s", id, val, bar('#', val, sum, graph_width).c_str());
       id += 1;
    }
}
