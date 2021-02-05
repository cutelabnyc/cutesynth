#include <stdint.h>

#define LUT_SIZE 1024

#ifdef __cplusplus
extern "C"
{
#endif

    extern const float cos_lut[];
    extern const int16_t cos_lut_fixed8[];
    extern const int32_t cos_lut_fixed16[];

#ifdef __cplusplus
}
#endif
