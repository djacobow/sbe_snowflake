///////////////////////////////////////////////
// 
// Copyright 2019 South Berkeley Electronics
// All Rights Reserved
//
// Arduino sketch to control an LED snowflake.
//
// Author: Dave Jacobowitz (dave@southberkeleyelectronics.com)
//
///////////////////////////////////////////////

#ifndef __BPRESS_H
#define __BPRESS_H

#include <Arduino.h>

typedef enum bp_t {
    bp_none = 0,
    bp_short,
    bp_medium,
    bp_long,
    bp_verylong,
} bp_t;

template<uint8_t PIN, uint16_t SHORT_TIME, uint16_t MED_TIME, uint16_t LONG_TIME>
class Button_c {
    private:
        uint8_t bvals;
        uint8_t last_bvals;
        uint32_t down_time;
    public:
        Button_c() : bvals(0xff), down_time(0) {
            pinMode(PIN,INPUT_PULLUP);
        };
        bp_t pressed() {
            uint32_t now = millis();
            bp_t rv = bp_none;
            bvals <<= 1;
            bvals |= digitalRead(PIN) ? 1 : 0;
            if (!bvals && last_bvals) {
                down_time = now;
            } else if (down_time && ((bvals & 0x0f) == 0x1)) {
                uint32_t duration = now - down_time;
                Serial.print("duration ");
                Serial.println(duration);
                rv = duration < SHORT_TIME ? bp_short :
                     duration < MED_TIME   ? bp_medium : 
                     duration < LONG_TIME  ? bp_long: bp_verylong;
                down_time = 0;
            }
            last_bvals = bvals;
            return rv;
        }
};

#endif
