#ifndef __fun_stuff_h
#define __fun_stuff_h

#include "pixchain.h"

// This file contains a base class and one subclass showing an effect.
// The base is to make it easy to have any number of effects, each of
// which could be called from the main code by the same shared functions,
// namely init() and tick(). The base class also holds a reference to 
// the pixelchain object, which actually has the buffers we're writing
// color data to.

template<class PIX_C>
class Effect_Base_c {
    public:
    Effect_Base_c(PIX_C &inp) : 
        pixels(inp) { };

    virtual void init() { };
    virtual void _tick() = 0;

    void tick() {
        _tick();
    };

    protected:
    PIX_C &pixels;
};


// Simple blue and white sparkle effect.
template<class PIX_C>
class Effect_Sparkle_c : public Effect_Base_c<PIX_C> {

    typedef Effect_Base_c<PIX_C> parent_t;

    public:

    Effect_Sparkle_c(PIX_C &inp) :
        parent_t(inp) {};

    void _tick() { 
        uint32_t a  = 0xffffffff;
        uint32_t b  = 0xffffffff;

        // generate two 32b numbers that have some random
        // value. We do it three times so on average, only
        // 1/8 of the bits are set.
        for (uint8_t i=0;i<3;i++) {
            a &= random();
            b &= random();
        }

        // now iterate through each of the pixels and determine
        // if it should be white, blue, or off depending on the
        // random bits we just made
        for (uint8_t i=0;i<parent_t::pixels.len();i++) {
            bool ab = a & 0x1;
            bool bb = b & 0x1;

            // create a pixel value. The arguments are for the red,
            // green, and blue channels, in order.
            // Red and green will be set only if bb is true, and 
            // blue will be set if either bb or ab are true. This way
            // we get either 0xff,0xff,0xff (white) or 0x0,0x0,0xff (blue),
            // or 0x0,0x0,0x0 (off)
            pixel_t np(bb ? 0xff: 0, bb ? 0xff : 0, (bb || ab) ? 0xff : 0);
            // then set the pixel
            parent_t::pixels.set(i,np);
            a >>= 1; b >>= 1;
        }
    };
};

#endif

