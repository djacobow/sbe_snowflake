#ifndef __sensors_h
#define __sensors_h

#include "debug.h"
#include "ema.h"

const uint32_t DEFAULT_SEED_V = 12345;

template<uint8_t LIGHT_PIN, uint8_t SOUND_PIN, uint8_t RAND_PIN, uint8_t RF_PIN>
class Sensors_c {
    public:
    Sensors_c() : z1(DEFAULT_SEED_V), z2(DEFAULT_SEED_V), z3(DEFAULT_SEED_V), z4(DEFAULT_SEED_V) {
        pinMode(LIGHT_PIN,INPUT);
        pinMode(SOUND_PIN,INPUT);
        pinMode(RAND_PIN,INPUT);
        pinMode(RF_PIN,INPUT);
        light_filter.init(analogRead(LIGHT_PIN));
        sound_filter.init(analogRead(SOUND_PIN));
#ifdef INCLUDE_RF
        rf_filter.init(analogRead(RF_PIN));
#endif
    }

    uint32_t rand32() {
        uint32_t b;
        b  = ((z1 << 6) ^ z1) >> 13;
        z1 = ((z1 & 4294967294U) << 18) ^ b;
        b  = ((z2 << 2) ^ z2) >> 27; 
        z2 = ((z2 & 4294967288U) << 2) ^ b;
        b  = ((z3 << 13) ^ z3) >> 21;
        z3 = ((z3 & 4294967280U) << 7) ^ b;
        b  = ((z4 << 3) ^ z4) >> 12;
        z4 = ((z4 & 4294967168U) << 13) ^ b;
        return (z1 ^ z2 ^ z3 ^ z4);
    }


    // reseeds the PRNG with a few real random numbers
    bool reseed() {
        z1 = trueRand32();
        z2 = trueRand32();
        z3 = trueRand32();
        z4 = trueRand32();
        DEBUG_PRINT_F("reseed ");
        DEBUG_PRINT_FMT(z1, HEX);
        DEBUG_PRINT_F(" ");
        DEBUG_PRINT_FMT(z2, HEX);
        DEBUG_PRINT_F(" ");
        DEBUG_PRINT_FMT(z3, HEX);
        DEBUG_PRINT_F(" ");
        DEBUG_PRINTLN_FMT(z4, HEX);
        // if all the same, that's bad
        bool ok = (z1 ^ z2 ^ z3 ^ z4) != 0;
        if (!ok) {
            z1 = DEFAULT_SEED_V;
            z2 = DEFAULT_SEED_V;
            z3 = DEFAULT_SEED_V;
            z4 = DEFAULT_SEED_V;

        }
        return ok;
    }

    // generates a "pretty good" True random number by
    // repeatedly resampling an unconnected input pin and 
    // using the noise in the LSBs. 
    // Probably not good enough for crypto purposes. :-)
    uint32_t trueRand32() {
        uint32_t nr = last_rand32;
        for (uint8_t i=0;i<4;i++) {
            nr ^= (uint32_t)_makeRandByte(5,3) << (i*8);
        }
        last_rand32 = nr;
        return nr;
    }


    uint16_t light() {
        return light_filter.update(analogRead(LIGHT_PIN));
    }
    uint16_t sound() {
        return sound_filter.update(analogRead(SOUND_PIN) << 4);
    }
#ifdef INCLUDE_RF
    uint8_t rf() {
        return rf_filter.update(analogRead(RF_PIN));
    }
#endif
    uint16_t myVcc() {
        // Read 1.1V reference against AVcc
        // cribbed from: 
        // https://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/

       // set the reference to Vcc and the measurement to the internal 1.1V reference
       #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
       ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
       #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
       ADMUX = _BV(MUX5) | _BV(MUX0);
       #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
       ADMUX = _BV(MUX3) | _BV(MUX2);
       #else
       ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
       #endif  

       delay(2);  // settle time
       ADCSRA |= _BV(ADSC);
       while (bit_is_set(ADCSRA,ADSC));
       uint16_t result = ADCL; // must read ADCL first - it then locks ADCH  
       result |= (ADCH << 8);
       result = 1125300L / result; // 1125300 = 1.1*1023*1000
       return result;
    }


    private:
    uint32_t z1, z2, z3, z4;
    uint32_t last_rand32;

    ema_c<uint16_t, uint32_t, 1, 32> light_filter;
    ema_c<uint16_t, uint32_t, 1, 16> sound_filter;
#ifdef INCLUDE_RF
    ema_c<uint8_t,  uint16_t, 1, 32> rf_filter;
#endif

    bool _makeRandBit(uint8_t iters) {
        bool r = 0;
        for (uint8_t i=0;i<iters;i++) {
            uint16_t x0 = analogRead(RAND_PIN);
            // bitwise xor reduction
            uint8_t  x1 = (x0 & 0xff) ^ ((x0 >> 8) & 0xff);
            uint8_t  x2 = (x1 & 0x0f) ^ ((x1 >> 4) & 0x0f);
            uint8_t  x3 = (x2 & 0x03) ^ ((x2 >> 2) & 0x03);
            uint8_t  x4 = (x3 & 0x01) ^ ((x3 >> 1) & 0x01);
            r ^= x4;
        }
        return r;
    }
    uint8_t _makeRandByte(uint8_t iters1, uint8_t iters0) {
        uint8_t ob = 0;
        for (uint8_t iter=0;iter<iters1;iter++) {
            uint8_t b = 0;
            for (uint8_t i=0;i<8;i++) {
                b <<= 1;
                b |= _makeRandBit(iters0);
            }
            ob ^= b;
        }
        return ob;
    }
};


#endif


