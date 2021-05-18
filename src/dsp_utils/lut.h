#include <stdint.h>

#define LUT_SIZE (uint16_t)1024
#define LUT_SIZE_HALF (LUT_SIZE >> 1)

#ifdef __cplusplus
extern "C"
{
#endif

    extern const float cos_lut[];
    extern const int16_t cos_lut_fixed8[];
    extern const int32_t cos_lut_fixed16[];

    extern const int16_t saw_lut_fixed8[];
    extern const int32_t saw_lut_fixed16[];

    extern const int16_t square_lut_fixed8[];
    extern const int32_t square_lut_fixed16[];

    extern const int16_t tri_lut_fixed8[];
    extern const int32_t tri_lut_fixed16[];

#ifdef __cplusplus
}
#endif
