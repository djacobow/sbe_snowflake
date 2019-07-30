#ifndef __pixchain_h
#define __pixchain_h

#include <stdint.h>
#include <Arduino.h>
#include "pixel.h"

template<uint8_t CHAIN_LENGTH, uint8_t OPIN>
class PixChain_c {

    private:
    pixel_t pixdata[CHAIN_LENGTH]; // holds working pixel data
    pixel_t outdata[CHAIN_LENGTH]; // holds output pixel data

    pixel_t temppixel;
    uint8_t safen(const uint8_t n) const {
        uint8_t on = n;
        if (on >= CHAIN_LENGTH) {
            on = on % CHAIN_LENGTH;
        }
        return on;
    }
    void _finish_setup() {
        pinMode(OPIN,OUTPUT);
        digitalWrite(OPIN,LOW);
    }
    public:

    PixChain_c() { 
        _finish_setup();
    }

    void disable() {
        pinMode(OPIN,INPUT);
    }

    // copy "working" buffer to output buffer, optionally applying
    // a mask and a brightness scaling factor
    void copyToOut(uint32_t mask = -1, uint8_t scale = -1) {

        uint32_t max_mask = (uint32_t)-1 >> (32-CHAIN_LENGTH);
        mask &= max_mask;

        for (uint8_t i=0;i<CHAIN_LENGTH;i++) {
            pixel_t np(0,0,0);

            
            if (mask & 0x1) {
                np = pixdata[i];
                np.scale(scale);
            }
            outdata[i] = np;
            mask >>= 1;
        }
    } 

    // turn off a pixel
    void clear(uint8_t n) {
        pixel_t p;
        set(n,p);
    }
    // turn of all the pixels
    void clear() {
        for (uint8_t i=0;i<CHAIN_LENGTH;i++) {
            clear(i);
        }
    }

    // set a specific pixel
    void set(uint8_t n, pixel_t p) {
        pixdata[safen(n)] = p;
        
    }
    void set(uint8_t n, uint8_t r, uint8_t g, uint8_t b) {
        pixel_t p(r,g,b);
        set(n,p);
    }
    void set(uint8_t n, uint32_t pixint) {
        pixel_t p(pixint);
        set(n,p);
    }

    // set all the pixels
    void setAll(pixel_t p) {
        for (uint8_t i=0;i<CHAIN_LENGTH;i++) {
            set(i,p);
        }
    };
    void setAll(uint32_t pixint) {
        pixel_t p(pixint);
        setAll(p);
    };
    void setAll(uint8_t r, uint8_t g, uint8_t b) {
        pixel_t p(r,g,b);
        setAll(p);
    }

    // scale a pixel
    void scale(uint8_t n, uint8_t s) {
        n = safen(n);
        pixel_t p = pixdata[n];
        p.scale(s);
        pixdata[n] = p;
    }
    void scaleAll(uint8_t s) {
        for (uint8_t i=0; i<CHAIN_LENGTH; i++) {
            scale(i,s);
        }
    }

    // see a pixel
    pixel_t get(uint8_t n) const {
        return pixdata[safen(n)];
    }
    uint8_t len() const {
        return CHAIN_LENGTH;
    }
    pixel_t *getAll() {
        return pixdata;
    }

    void dump() {
        for (uint8_t i=0;i<CHAIN_LENGTH;i++) {
            Serial.print(F("# "));
            Serial.print(i);
            Serial.print(F(": "));
            pixdata[i].dump();
        }
    }

    void show() const {
        noInterrupts();

        volatile uint8_t *port = &PORTD;
        volatile uint8_t *ptr = (uint8_t *)(void *)outdata;        
        volatile uint8_t b = *ptr++;
        uint8_t pinmask = 0x1 << OPIN;
        volatile uint8_t hi = *port | pinmask;
        volatile uint8_t lo = *port & ~pinmask;
        volatile uint16_t i = sizeof(outdata);

#if F_CPU >= 14000000 && F_CPU <= 19000000
        volatile uint8_t next, bit;

        next = lo;
        bit  = 8;
        

        // cribbed from adafruit lib
        // This is only good for a 16 MHz AVR.
        asm volatile(
         "head20:"                   "\n\t" // Clk  Pseudocode    (T =  0)
          "st   %a[port],  %[hi]"    "\n\t" // 2    PORT = hi     (T =  2)
          "sbrc %[byte],  7"         "\n\t" // 1-2  if(b & 128)
          "mov  %[next], %[hi]"     "\n\t" // 0-1   next = hi    (T =  4)
          "dec  %[bit]"              "\n\t" // 1    bit--         (T =  5)
          "st   %a[port],  %[next]"  "\n\t" // 2    PORT = next   (T =  7)
          "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T =  8)
          "breq nextbyte20"          "\n\t" // 1-2  if(bit == 0) (from dec above)
          "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 10)
          "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 12)
          "nop"                      "\n\t" // 1    nop           (T = 13)
          "st   %a[port],  %[lo]"    "\n\t" // 2    PORT = lo     (T = 15)
          "nop"                      "\n\t" // 1    nop           (T = 16)
          "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 18)
          "rjmp head20"              "\n\t" // 2    -> head20 (next bit out)
         "nextbyte20:"               "\n\t" //                    (T = 10)
          "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 11)
          "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 13)
          "st   %a[port], %[lo]"     "\n\t" // 2    PORT = lo     (T = 15)
          "nop"                      "\n\t" // 1    nop           (T = 16)
          "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 18)
           "brne head20"             "\n"   // 2    if(i != 0) -> (next byte)
          : [port]  "+e" (port),
            [byte]  "+r" (b),
            [bit]   "+r" (bit),
            [next]  "+r" (next),
            [count] "+w" (i)
          : [ptr]    "e" (ptr),
            [hi]     "r" (hi),
            [lo]     "r" (lo)
        );

#endif

#if F_CPU > 7000000 && F_CPU <= 9000000

        volatile uint8_t n1, n2 = 0;  // First, next bits out
        if (false) {
            uint8_t tmp = hi;
            hi = lo;
            lo = tmp; 
            n1 = lo;
        }

        if(b & 0x80) n1 = hi;

        asm volatile(
       "headD:"                   "\n\t" // Clk  Pseudocode
        // Bit 7:
        "out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
        "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
        "out  %[port] , %[n1]"    "\n\t" // 1    PORT = n1
        "rjmp .+0"                "\n\t" // 2    nop nop
        "sbrc %[byte] , 6"        "\n\t" // 1-2  if(b & 0x40)
         "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
        "out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
        "rjmp .+0"                "\n\t" // 2    nop nop
        // Bit 6:
        "out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
        "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
        "out  %[port] , %[n2]"    "\n\t" // 1    PORT = n2
        "rjmp .+0"                "\n\t" // 2    nop nop
        "sbrc %[byte] , 5"        "\n\t" // 1-2  if(b & 0x20)
         "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
        "out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
        "rjmp .+0"                "\n\t" // 2    nop nop
        // Bit 5:
        "out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
        "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
        "out  %[port] , %[n1]"    "\n\t" // 1    PORT = n1
        "rjmp .+0"                "\n\t" // 2    nop nop
        "sbrc %[byte] , 4"        "\n\t" // 1-2  if(b & 0x10)
         "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
        "out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
        "rjmp .+0"                "\n\t" // 2    nop nop
        // Bit 4:
        "out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
        "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
        "out  %[port] , %[n2]"    "\n\t" // 1    PORT = n2
        "rjmp .+0"                "\n\t" // 2    nop nop
        "sbrc %[byte] , 3"        "\n\t" // 1-2  if(b & 0x08)
         "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
        "out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
        "rjmp .+0"                "\n\t" // 2    nop nop
        // Bit 3:
        "out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
        "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
        "out  %[port] , %[n1]"    "\n\t" // 1    PORT = n1
        "rjmp .+0"                "\n\t" // 2    nop nop
        "sbrc %[byte] , 2"        "\n\t" // 1-2  if(b & 0x04)
         "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
        "out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
        "rjmp .+0"                "\n\t" // 2    nop nop
        // Bit 2:
        "out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
        "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
        "out  %[port] , %[n2]"    "\n\t" // 1    PORT = n2
        "rjmp .+0"                "\n\t" // 2    nop nop
        "sbrc %[byte] , 1"        "\n\t" // 1-2  if(b & 0x02)
         "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
        "out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
        "rjmp .+0"                "\n\t" // 2    nop nop
        // Bit 1:
        "out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
        "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
        "out  %[port] , %[n1]"    "\n\t" // 1    PORT = n1
        "rjmp .+0"                "\n\t" // 2    nop nop
        "sbrc %[byte] , 0"        "\n\t" // 1-2  if(b & 0x01)
         "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
        "out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
        "sbiw %[count], 1"        "\n\t" // 2    i-- (don't act on Z flag yet)
        // Bit 0:
        "out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
        "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
        "out  %[port] , %[n2]"    "\n\t" // 1    PORT = n2
        "ld   %[byte] , %a[ptr]+" "\n\t" // 2    b = *ptr++
        "sbrc %[byte] , 7"        "\n\t" // 1-2  if(b & 0x80)
         "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
        "out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
        "brne headD"              "\n"   // 2    while(i) (Z flag set above)
      : [byte]  "+r" (b),
        [n1]    "+r" (n1),
        [n2]    "+r" (n2),
        [count] "+w" (i)
      : [port]   "I" (_SFR_IO_ADDR(PORTD)),
        [ptr]    "e" (ptr),
        [hi]     "r" (hi),
        [lo]     "r" (lo));

#endif

        interrupts();
    };


    pixel_t average(pixel_t a, pixel_t b) {
        pixel_t o;
        for (uint8_t i=0;i<sizeof(o.d);i++) {
            uint16_t sum = (uint16_t)a.d[i] + (uint16_t)b.d[i];
            sum >>= 1;
            o.d[i] = sum;
        }
        return o;
    };

    pixel_t average(uint8_t a, uint8_t b) {
        return average(pixdata[safen(a)],pixdata[safen(b)]);
    };
};

#endif

