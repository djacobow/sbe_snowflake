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

#ifndef __helpers_h
#define __helpers_h

template<typename T, int size>
int getLength(T(&)[size]){return size;}

template<typename T>
T wrapDecr(T &in, uint8_t len) {
    if (!in) in = len-1;
    else in -= 1;
    return in;
};

template<typename T>
T wrapIncr(T &in, uint8_t len) {
    in += 1;
    if (in >= len) in = 0;
    return in;
};

uint8_t  fromProgMem8(const uint8_t   *pbase, uint8_t idx);
uint16_t fromProgMem16(const uint16_t *pbase, uint8_t idx);
uint32_t fromProgMem32(const uint32_t *pbase, uint8_t idx);

// fast base2 log using lookup table
uint32_t log2int(uint32_t in);

// routine to take a time series and return true when
// a given sample is larger than the average
uint32_t thresholder(uint16_t in);

const uint8_t PROGMEM cylons[] = {
      1,  2, 27, 28 ,
      0,  3, 29, 26 ,
      4, 25,  4, 25 ,
      5,  6, 23, 24 ,
      7, 22,  7, 22 ,
      8,  9, 20, 21 ,
     10, 19, 10, 19 ,
     11, 14, 15, 18 ,
     12, 13, 16, 17 ,
};

#define USE_LOG_SCALE 

// routine to scale a limited range to a mask
template<uint16_t in_min, uint16_t in_max, uint8_t CHAIN_LENGTH, uint32_t ALL_MSK>
uint32_t scale2mask(uint16_t in) {
#ifdef USE_LOG_SCALE
    const uint32_t in_range = in_max - in_min;
    if (in >= in_max) in = in_max;
    if (in < in_min) in = in_min;
    uint32_t fract = (uint32_t)in << 16;
    fract /= in_range;
    uint16_t  l2f   = log2int(fract);
    l2f *= CHAIN_LENGTH;
    l2f >>= 4;
    uint32_t omsk = ALL_MSK >> (CHAIN_LENGTH-l2f);
#else
    if (in > in_max) in = in_max;
    if (in < in_min) in = in_min;
    static uint16_t max_seen = in_min;
    if (in > max_seen) max_seen = in;
    DEBUG_PVAR(max_seen);
    DEBUG_PVAR(in);
    uint32_t fract = ((uint32_t)in << 8) / (in_max-in_min);
    uint8_t  shift = fract / (255/CHAIN_LENGTH);
    uint32_t omsk = ALL_MSK >> (CHAIN_LENGTH-shift);
#endif
    return omsk;
};

int freeRam();

#endif


