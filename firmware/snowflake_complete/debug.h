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

#ifndef __DEBUG_H
#define __DEBUG_H

#define SERIAL_DEBUG

#ifdef SERIAL_DEBUG
    #define DEBUG_FLUSH() \
        Serial.flush()

    #define DEBUG_INIT(speed) \
        Serial.begin(speed); \
        Serial.println(F("debug init!"))

    #define DEBUG_PRINTLN_FMT(v,f) \
        Serial.println(v,f)

    #define DEBUG_PRINTLN(v) \
        Serial.println(v)

    #define DEBUG_PRINTLN_F(v) \
        Serial.println(F(v))

    #define DEBUG_PRINT_FMT(v,f) \
        Serial.print(v,f)

    #define DEBUG_PRINT(v) \
        Serial.print(v)

    #define DEBUG_PRINT_F(v) \
        Serial.print(F(v))

    #define DEBUG_PVAR_FMT(v,f) \
        Serial.print(F(#v ": ")); \
        Serial.println(v,f)

    #define DEBUG_PVAR(v) \
        DEBUG_PVAR_FMT(v,DEC)


#else
    #define DEBUG_INIT(speed)
    #define DEBUG_PRINTLN(v)
    #define DEBUG_PRINTLN_F(v)
    #define DEBUG_PRINTLN_FMT(v,f)
    #define DEBUG_PRINT(v)
    #define DEBUG_PRINT_F(v)
    #define DEBUG_PRINT_FMT(v,f)
    #define DEBUG_PVAR(v)
    #define DEBUG_PVAR_F(v)
    #define DEBUG_PVAR_FMT(v,f)
    #define DEBUG_FLUSH()
#endif


#endif

