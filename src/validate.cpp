#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <string>

#include "input_generator.h"

int main() {

    const auto tmp = generate(1024, 5, 4);

    puts(tmp.c_str());
}
