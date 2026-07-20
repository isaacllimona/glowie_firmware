#pragma once

#include <assert.h>

#define CONFIG_GLW_LED_GPIO 9
#define CONFIG_GLW_LED_NUM  32

#define CONFIG_GLW_LED_NUM_PIXELS 32
#define CONFIG_GLW_LED_PIXEL_MAPPING {              \
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,          \
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,         \
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,         \
    30, 31                                          \
}

static_assert(
    sizeof((char[])CONFIG_GLW_LED_PIXEL_MAPPING) == CONFIG_GLW_LED_NUM_PIXELS,
    "The length of the pixel mapping array does not match the specified number of pixels"
);
