#include <stdint.h>

#define LUT_SIZE (uint16_t)1024
#define LUT_SIZE_HALF (LUT_SIZE >> 1)

#ifdef __cplusplus
extern "C"
{
#endif

    extern const uint16_t cos_lut_fixed8[];
    extern const uint32_t cos_lut_fixed16[];

    extern const uint16_t saw_lut_fixed8[];
    extern const uint32_t saw_lut_fixed16[];

    extern const uint16_t square_lut_fixed8[];
    extern const uint32_t square_lut_fixed16[];

    extern const uint16_t tri_lut_fixed8[];
    extern const uint32_t tri_lut_fixed16[];

#ifdef __cplusplus
}
#endif
