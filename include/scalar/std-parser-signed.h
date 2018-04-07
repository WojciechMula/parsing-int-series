#pragma once

#include <cstdlib>
#include <cstring>
#include <climits>
#include <stdexcept>

namespace scalar {

    namespace cstd {

        template <typename INSERTER>
        void parse_signed(const char* data, size_t size, const char* separators, INSERTER output) {

            char* ptr = const_cast<char*>(data);
            char* end = ptr + size;

            char* endptr;
            while (true) {
                ptr += strspn(ptr, separators);
                if (ptr == end) {
                    break;
                }

                errno = 0;
                const long val = std::strtol(ptr, &endptr, 10);

                // the following check comes from "man 3 strtol"
                if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
                    throw std::logic_error("invalid input");
                }

                if (endptr == ptr) {
                    throw std::logic_error("no digits");
                }

                ptr = endptr;
                *output++ = val;
            }
        }
    }
}
