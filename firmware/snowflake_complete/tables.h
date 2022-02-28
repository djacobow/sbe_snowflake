
#ifndef __tables_h
#define __tables_h

#include <stdint.h>

uint8_t sine8(uint8_t x);
uint8_t gamma8(uint8_t x);

typedef struct color3_t {
     uint8_t d[3];
} color3_t;

typedef enum pixel_color_t {
    P_BLACK = 0,
    P_WHITE,
    P_RED,
    P_LIME,
    P_BLUE,
    P_YELLOW,
    P_CYAN,
    P_MAGENTA,
    P_SILVER,
    P_GRAY,
    P_MAROON,
    P_OLIVE,
    P_GREEN,
    P_PURPLE,
    P_TEAL,
    P_NAVY,
} pixel_color_t;

color3_t getColor(const pixel_color_t pcolor = P_BLACK);

#endif

