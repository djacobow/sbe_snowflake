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

#include <stdint.h>
#include <Arduino.h>
#include "helpers.h"
#include "ema.h"

uint8_t fromProgMem8(const uint8_t *pbase, uint8_t idx) {
    return pgm_read_byte(pbase + idx);
}
uint16_t fromProgMem16(const uint16_t *pbase, uint8_t idx) {
    return pgm_read_word(pbase + idx);
}
uint32_t fromProgMem32(const uint32_t *pbase, uint8_t idx) {
    return pgm_read_dword(pbase + idx);
}


// cribbed from: https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog

static const uint8_t PROGMEM _magic_log_constants[32] = {
  0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
  8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
};



uint32_t log2int(uint32_t v) {
    v |= v >> 1; // first round down to one less than a power of 2
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    return fromProgMem8(_magic_log_constants,(uint32_t)(v * 0x07C4ACDDU) >> 27);
}

uint32_t thresholder(uint16_t in) {
    static ema_c<uint16_t, uint32_t, 1, 64>  avg_filter;
    uint16_t avg = avg_filter.update(in);
    return (in > avg);
};


int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

