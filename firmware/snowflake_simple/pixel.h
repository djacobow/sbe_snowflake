#ifndef __pixel_h
#define __pixel_h

class pixel_t {
    public:
    // NB: WS2812B,C and SK6812 go Green, Red, Blue
    uint8_t d[3];

    pixel_t() {
        memset(d,0,sizeof(d));
    }
    
    pixel_t(uint8_t ir, uint8_t ig, uint8_t ib) {
        d[0] = ig; d[1] = ir; d[2] = ib;
    };

    pixel_t(uint32_t pixint) {
        for (uint8_t i=0;i<sizeof(d);i++) {
            d[i] = pixint & 0xff;
            pixint >>= 8;
        }
    }

    // scale a pixel with a value 0-255
    void scale(uint8_t s) {
        for (uint8_t i=0;i<sizeof(d);i++) {
            uint16_t wv = d[i];
            wv *= s;
            wv >>= 8;
            d[i] = wv;
        }
    };

    // sum two pixels
    void add(pixel_t addend) {
        for (uint8_t i=0;i<sizeof(d);i++) {
            uint8_t this_v = d[i];
            uint8_t othr_v = addend.d[i];
            uint8_t sum = this_v + othr_v;
            if (sum<this_v) {
                // if overflow, then max
                sum = -1;
            }
            d[i] = sum;
        }
    };

    // mix two pixels according to the fraction of each
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
            Serial.print(this_c);
            Serial.print(F("(0x"));
            Serial.print(this_v,HEX);
            if (i == sizeof(d)-1) {
                Serial.println(F(")"));
            } else {
                Serial.print(F("), "));
            }
        }
    }
};


#endif

