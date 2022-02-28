#ifndef __pixel_h
#define __pixel_h

#include "debug.h"
#include "tables.h"

class pixel_t {
    public:
    // NB: WS2812B and SK6812 go G, R, B
    uint8_t d[3];

    pixel_t() {
        memset(d,0,sizeof(d));
    }
    
    pixel_t(const pixel_color_t &pc) {
        color3_t c = getColor(pc);
        d[0] = c.d[1];
        d[1] = c.d[0];
        d[2] = c.d[2];
    }

    pixel_t(uint8_t ir, uint8_t ig, uint8_t ib) {
        // This is a strange order. It's just how the pixels work.
        d[0] = ig;
        d[1] = ir;
        d[2] = ib;
    };

    pixel_t(uint32_t pixint) {
        for (uint8_t i=0;i<sizeof(d);i++) {
            d[i] = pixint & 0xff;
            pixint >>= 8;
        }
    }

    void scale(uint8_t s) {
        for (uint8_t i=0;i<sizeof(d);i++) {
            uint16_t wv = d[i];
            wv *= s;
            wv >>= 8;
            d[i] = wv;
        }
    };

    void add(pixel_t addend) {
        for (uint8_t i=0;i<sizeof(d);i++) {
            uint8_t this_v = d[i];
            uint8_t othr_v = addend.d[i];
            uint8_t sum = this_v + othr_v;
            if (sum<this_v) {
                sum = -1;
            }
            d[i] = sum;
        }
    };

    void mix(pixel_t l, pixel_t r, uint8_t fract) {
        for (uint8_t i=0;i<sizeof(d);i++) {
            uint16_t bl = 0;
            bl += (uint16_t)l.d[i] * (uint16_t)(255-fract);
            bl += (uint16_t)r.d[i] * (uint16_t)fract;
            bl >>= 8;
            d[i] = bl;
        }
    };

    void dump() {
        const char* names[] = {"red","green","blue"};
        for (uint8_t i=0;i<sizeof(d);i++) {
            uint8_t this_v = d[i];
            const char *this_c = names[i];
            DEBUG_PRINT(this_c);
            DEBUG_PRINT(" 0x");
            DEBUG_PRINT_FMT(this_v,HEX);
            if (i == sizeof(d)-1) {
                DEBUG_PRINTLN();
            } else {
                DEBUG_PRINT(", ");
            }
        }
    }
};


#endif

