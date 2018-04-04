#include "sse/sse-matcher-stni.h"

#include <string>
#include <cassert>

bool all_bytes_equal(__m128i a, __m128i b) {
    
    uint8_t tmpa[16];
    uint8_t tmpb[16];

    _mm_storeu_si128(reinterpret_cast<__m128i*>(tmpa), a);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(tmpb), b);

    return memcmp(tmpa, tmpb, 16) == 0;
}

int main() {

    sse::STNIMatcher matcher(",.;");

    const std::string s = ".123,45.;091;.;,";
    assert(s.size() == 16);

    const __m128i input = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s.c_str()));
    const __m128i ret   = matcher.get_mask(input, _mm_setzero_si128());

    const __m128i expected = _mm_setr_epi8(-1, 0, 0, 0, -1, 0, 0, -1, -1, 0, 0, 0, -1, -1, -1, -1);

    assert(all_bytes_equal(ret, expected));
}
