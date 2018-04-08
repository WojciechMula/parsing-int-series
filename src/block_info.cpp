#include "block_info.h"

#include "block_info.inl"

namespace {
    
    void as_array(FILE* f, const uint8_t data[16]) {
        fprintf(f, "{");
        fprintf(f, "%02x", data[0]);
        for (int i=1; i < 16; i++)
            fprintf(f, ", %02x", data[i]);
        fprintf(f, "}\n");
    }

    const char* to_string(Conversion c) {
        switch (c) {
            case Conversion::Scalar:
                return "Scalar";

            case Conversion::SSE1Digit:
                return "SSE1Digit";

            case Conversion::SSE2Digits:
                return "SSE2Digits";

            case Conversion::SSE4Digits:
                return "SSE4Digits";

            case Conversion::SSE8Digits:
                return "SSE8Digits";

            default:
                return "<unknown>";
        }
    }

} // namespace
 
void BlockInfo::dump(FILE* f) const {
    fprintf(f, "first_skip          : %d\n", first_skip);
    fprintf(f, "total_skip          : %d\n", total_skip);
    fprintf(f, "element_count       : %d\n", element_count);
    fprintf(f, "conversion          : %s\n", to_string(conversion_routine));
    fprintf(f, "invalid_sign_mask   : %04x\n", invalid_sign_mask);
    fprintf(f, "shuffle_digits      : "); as_array(f, shuffle_digits);
    fprintf(f, "shuffle_signs       : "); as_array(f, shuffle_signs);
}
