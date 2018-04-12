#pragma once

namespace scalar {

    bool contains(const char* set, char c) {
        char* s = const_cast<char*>(set);
        while (*s) {
            if (*s++ == c) {
                return true;
            }
        }

        return false;
    }

}
