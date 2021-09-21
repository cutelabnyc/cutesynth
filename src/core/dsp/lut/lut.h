#include <stdint.h>

#define LUT_SIZE (uint16_t)1024
#define LUT_SIZE_HALF (LUT_SIZE >> 1)

#ifdef __cplusplus
extern "C"
{
#endif

    extern const uint8_t sin_lut_uint8[];
    extern const uint16_t sin_lut_uint16[];

    extern const uint8_t saw_lut_uint8[];
    extern const uint16_t saw_lut_uint16[];

    extern const uint8_t square_lut_uint8[];
    extern const uint16_t square_lut_uint16[];

    extern const uint8_t tri_lut_uint8[];
    extern const uint16_t tri_lut_uint16[];

#ifdef __cplusplus
}
#endif
