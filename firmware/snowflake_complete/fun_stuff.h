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

#ifndef __fun_stuff_h
#define __fun_stuff_h

#include "pixchain.h"
#include "helpers.h"
#include "tables.h"
#include "helpers.h"

template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Base_c {
    public:
    Fun_Base_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) : 
        pixels(inp), sensors(insens), varns(invarns) { };

    virtual void init() { };
    virtual void _tick() = 0;

    void tick() {
        _tick();
    };

    protected:
    PIX_C &pixels;
    SENS_C &sensors;
    VARNS_C &varns;
};


// blue and white sparkles
template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Sparkle_c : public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {

    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:

    Fun_Sparkle_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns) {};

    void _tick() { 
        if (true) {
            uint32_t a  = 0xffffffff;
            uint32_t b  = 0xffffffff;

            for (uint8_t i=0;i<parent_t::varns.var0_idx+1;i++) {
                a &= parent_t::sensors.rand32();
                b &= parent_t::sensors.rand32();
            };

            for (uint8_t i=0;i<parent_t::pixels.len();i++) {
                bool ab = a & 0x1;
                bool bb = b & 0x1;

                pixel_t np(bb ? 0xff: 0, bb ? 0xff : 0, (bb || ab) ? 0xff : 0);
                parent_t::pixels.set(i,np);
                a >>= 1; b >>= 1;
            }
        }
    };
};

// single lit pixel, random colors jumping around
template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Sparse_c : public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {

    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:

    Fun_Sparse_c (PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens, invarns), ttl(0), last_victim(0) {};

    void init() {
        parent_t::pixels.clear();
    }
    void _tick() { 
        if (!ttl) {
            parent_t::pixels.clear(last_victim);
            ttl = 0 + (parent_t::sensors.rand32() & 0x3);
            uint8_t victim = parent_t::sensors.rand32() % parent_t::pixels.len();
            uint32_t color = parent_t::sensors.rand32();
            pixel_t np(color);
            parent_t::pixels.set(victim,np);
            last_victim = victim;
            return;
        };
        ttl -= 1;
        
    };
    private:
        uint8_t ttl;
        uint8_t last_victim;

};

// random colors zooming around the inner and outer ring independently
template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Chaser_c: public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {

    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:

    Fun_Chaser_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns) {};

    void _tick() { 
        uint8_t dirmsk = 0x3 & parent_t::varns.var0_idx;

        if (true) {
            uint32_t a  = parent_t::sensors.rand32();

            if (true) {
                if ((a & 0xff) < 0x10) {
                    pixel_t np(parent_t::sensors.rand32());
                    parent_t::pixels.set(0,np);
                } else if ((a & 0xff) < 0x40) {
                    parent_t::pixels.clear(0);
                }
                parent_t::pixels.rotate(1,dirmsk & 0x1,ROTATE_INNER);
            }

            if (true) {
                if (((a >> 8)& 0xff) < 0x10) {
                    pixel_t np(parent_t::sensors.rand32());
                    parent_t::pixels.set(1,np);
                } else if (((a >> 8) & 0xff) < 0x40) {
                    parent_t::pixels.clear(1);
                }
                parent_t::pixels.rotate(1,dirmsk & 0x2,ROTATE_OUTER);
            }
        }
    };

    private:
    
};

const uint8_t  g_phases[]     PROGMEM = {  42,   0, 128,   0, 128, 85, 170, 0   };
const uint8_t  b_phases[]     PROGMEM = {  85,   0, 128, 128, 0,   42, 0,   160 };

// smooth color changing colors of all pixels
template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Rainbow_c: public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {

    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:

    Fun_Rainbow_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns), count(0) {};

    void _tick() { 

        uint8_t idx = parent_t::varns.var0_idx & 0x7;
        uint8_t g_phase = pgm_read_byte(g_phases + idx);
        uint8_t b_phase = pgm_read_byte(b_phases + idx);

        if (true) {
            for (uint8_t i=0;i<parent_t::pixels.len();i++) {
                uint8_t r = sine8(count + 8*i);
                uint8_t g = sine8(count + g_phase + 8*i);
                uint8_t b = sine8(count + b_phase + 8*i);
                parent_t::pixels.set(i,r,g,b);
            }
        }
        count += 1;
    };

    private:
    uint32_t count;

};

template<class PIX_C, class SENS_C, class VARNS_C, uint8_t PULSE_LEN>
class Fun_Snake_c: public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {

    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:

    Fun_Snake_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns), start(0), 
        just_inited(true) { };

    void _init() {
        just_inited = true;
    }
    void _tick() { 
        uint32_t a = parent_t::sensors.rand32();
        if (just_inited || ((a & 0xff) < 0x05)) {
            just_inited = false;
            uint32_t rcolor = parent_t::sensors.rand32();

            for (uint8_t i=0;i<PULSE_LEN;i++) {
                int8_t  center = PULSE_LEN >> 1;
                int8_t distance = center-i;
                if (distance < 0) distance = -distance;
                uint8_t scale_factor = 0xff >> (1*distance);
                pixel_t np(rcolor);
                np.scale(scale_factor);
                cp_color[i] = np;
               
            }
        } 
      
        for (uint8_t i=0;i<PULSE_LEN+2;i++) {
            uint8_t pidx = start + i;
            if ((i==0) || (i==PULSE_LEN+1)) {
                parent_t::pixels.set(pidx,0);
            } else {
                parent_t::pixels.set(pidx,cp_color[i-1]);
            }

        }
        wrapIncr(start,parent_t::pixels.len());
    };

    private:
        pixel_t cp_color[PULSE_LEN];
        uint8_t start;
        bool just_inited;

};



// each of the 6 arms of the snowflake a separate solid color
template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Leaves_c: public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {

    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:

    Fun_Leaves_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns) { };

    void _tick() { 
        uint8_t group = parent_t::sensors.rand32() & 0x7;
        if (group < 7) {
            bool use_black = !(parent_t::sensors.rand32() & 0xf);
            pixel_t np(use_black ? 0 : parent_t::sensors.rand32());
            for (uint8_t i=0;i<PIXELS_PER_LEAF;i++) {
                parent_t::pixels.set(i+group*PIXELS_PER_LEAF,np);
            }
        }
    };

    private:
        const uint8_t PIXELS_PER_LEAF = 5;

};



// all the LEDs the same color, changing regularly
template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Flash_c: public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {

    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:

    Fun_Flash_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns), ttw(0), ttl(0) {};

    void _tick() { 
        if (ttw) ttw -= 1;
        else if (!ttl) {
            // start a flash
            pixel_t np(P_WHITE);
            parent_t::pixels.setAll(np);
            ttl = parent_t::sensors.rand32() & 0xf;
        } else {
            ttl -= 1;
            if (!ttl) {
                ttw = parent_t::sensors.rand32() & 0x3f;
                parent_t::pixels.clear();
            }
        }
    };

    private:
        uint8_t ttw;
        uint8_t ttl;

};


template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Cylon_c: public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {

    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:

    Fun_Cylon_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns), row(0), orient(0), dir(false), pc(P_WHITE) {};

    void _tick() { 
        if (dir) {
            row -= 1;
            // we want to hit zero twice, as we return to the first row
            // on this orientation and start from 0 row on the next
            if (row>getLength(cylons)) {
                row = 0;
                dir = false;
                pc = (pixel_color_t)(pc + 1);
                if (pc > P_NAVY) pc = P_BLACK;
                orient += 1;
                if (orient > 5) orient = 0;
            }
        }  else {
            row += 1;
            // I wrote this code awhile go and I don't understand why it works
            if (row == (getLength(cylons)/4-1) ) {
                dir = true;
                if (parent_t::varns.var0_idx & 0x1) {
                    pc = (pixel_color_t)(pc + 1);
                    if (pc > P_NAVY) pc = P_BLACK;
                }
            }
        }

        if (!(parent_t::varns.var0_idx & 0x1)) {
            parent_t::pixels.clear();
        }

        for (uint8_t i=0;i<4;i+=1) {
            uint8_t loc = fromProgMem8(cylons,row*4 +i);
            loc += orient * 5;
            loc = loc % parent_t::pixels.len();
            parent_t::pixels.set(loc, pixel_t(pc));
        }
    };

    private:
        uint8_t row;
        uint8_t orient;
        bool dir;
        pixel_color_t pc; 
};


template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Solid_c : public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {

    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:

    Fun_Solid_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns), divcount(0) {};

    void _tick() { 
        if (!divcount) {
            uint32_t color = parent_t::sensors.rand32();
            pixel_t np(color);
            parent_t::pixels.setAll(np);
        }
        divcount += 1;
        divcount &= 0x7;
    };

    private:
        uint8_t divcount;
};


template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_MiniCircle_c : public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {

    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:

    Fun_MiniCircle_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns) {};

    void _tick() { 
        for (uint8_t i=0;i<parent_t::pixels.len()/PIXELS_PER_LEAF;i++) {
            for (uint8_t j=0;j<PIXELS_PER_LEAF;j++) {
                parent_t::pixels.set(i*PIXELS_PER_LEAF+j, colors[(j+count) % PIXELS_PER_LEAF]);
            }
        }
        if (!(count % (3*(parent_t::varns.var0_idx+1)))) {
            for (uint8_t i=0;i<PIXELS_PER_LEAF-1;i++) {
                colors[i] = colors[i+1];
            }
            pixel_t np(parent_t::sensors.rand32());
            colors[PIXELS_PER_LEAF-1] = np;
        }
        count += 1;
    };

    private:
        const uint8_t PIXELS_PER_LEAF = 5;
        pixel_t colors[5];
        uint8_t count;
};

const uint8_t  cuts0[4] PROGMEM = {  9,  9,  7,  7,  };
const uint8_t  cuts1[4] PROGMEM = {  12, 13, 10, 12, };

template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Inching_c: public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {

    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:

    Fun_Inching_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns), pidxs{0,1,2} {};

    void init() {
        parent_t::pixels.clear();
        cp = pixel_t(parent_t::sensors.rand32());
    }
    void _tick() {
        uint8_t dirmsk = 0x3 & parent_t::varns.var0_idx;
        uint8_t move_size = parent_t::sensors.rand32() & 0xf;

        if (move_size < pgm_read_byte(cuts0 + dirmsk)) {
            // stay
        } else if (move_size < pgm_read_byte(cuts1 + dirmsk)) {
            // move back 
            for (uint8_t i=0;i<3;i++) {
                pidxs[i] -= 1;
                if (pidxs[i] > parent_t::pixels.len()-1) pidxs[i] = parent_t::pixels.len()-1;
            }
        } else {
            // move forward 
            for (uint8_t i=0;i<3;i++) {
                pidxs[i] += 1;
                if (pidxs[i] > parent_t::pixels.len()-1) pidxs[i] = 0;
            }
        }

        if (!(parent_t::sensors.rand32() & 0xf)) {
            cp = pixel_t(parent_t::sensors.rand32());
        } 
        for (uint8_t i=0;i<3;i++) {
            uint8_t offset = i * (parent_t::pixels.len() / 3);
            parent_t::pixels.set(pidxs[0] + offset,0);
            parent_t::pixels.set(pidxs[1] + offset,cp);
            parent_t::pixels.set(pidxs[2] + offset,0);
        }
    }

    private:
        uint8_t pidxs[3];
        pixel_t cp;
};


template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Pulse_c : public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {
    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:
    Fun_Pulse_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns), dir(false), bright(0), color(0) {};

    void _tick() {
        if (!bright) color = pixel_t(parent_t::sensors.rand32());
        pixel_t adjColor = color;
        adjColor.scale(bright);
        parent_t::pixels.setAll(adjColor);
        uint8_t before_bright = bright;
        if (dir) {
            bright -= (parent_t::varns.var0_idx + 1);
            if (bright > before_bright) {
                bright = 0;
                dir = false;
            }
        } else {
            bright += (parent_t::varns.var0_idx + 1);
            if (bright < before_bright) {
                bright = 255;
                dir = true;
            }
        }
    }

    private:
        bool dir;
        uint8_t bright;
        pixel_t color;

};



template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Fade_c : public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {
    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:
    Fun_Fade_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns), progress(0) {};

    void init() {
        for (uint8_t i=0;i<MAX_CHUNKS+1;i++) {
            colors[i] = pixel_t(parent_t::sensors.rand32());
        }
    }
    void _tick() {
        uint8_t chunks = 1 + parent_t::varns.var0_idx;
        if (chunks > MAX_CHUNKS) chunks = MAX_CHUNKS;
        uint8_t pels_per_chunk = parent_t::pixels.len() / chunks;
        uint8_t pixnum = 0;
        for (uint8_t i=0;i<chunks;i++) {
            pixel_t blended;
            blended.mix(colors[i],colors[i+1],progress);
            for (uint8_t j=0;j<pels_per_chunk;j++) {
                parent_t::pixels.set(pixnum, blended);
                pixnum++;
            }
            if (i==chunks-1) {
                while (pixnum < parent_t::pixels.len()) {
                    parent_t::pixels.set(pixnum++, blended);
                }
            }
        }
        progress += 2;
        if (!progress) {
            for (uint8_t i=0;i<MAX_CHUNKS-1;i++) {
                colors[i] = colors[i+1];
            }
            colors[MAX_CHUNKS-1] = pixel_t(parent_t::sensors.rand32());
        }
    }
    private:
        static const uint8_t MAX_CHUNKS = 9;
        uint8_t progress;
        pixel_t colors[MAX_CHUNKS+1];

};




template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Settings_c : public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {

    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:

    Fun_Settings_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns) {};

    void _tick() {
        parent_t::pixels.clear();
        parent_t::pixels.set(parent_t::varns.delay_idx, P_NAVY);
        parent_t::pixels.set(parent_t::varns.turnoff_idx + 2*PIXELS_PER_LEAF, P_MAGENTA);
        parent_t::pixels.set(parent_t::varns.brite_idx + 4*PIXELS_PER_LEAF, P_TEAL);
    };
    private:
        const uint8_t PIXELS_PER_LEAF = 5;
};


const uint8_t  line_elems[4] PROGMEM = {  1,4,5,8 };

template<class PIX_C, class SENS_C, class VARNS_C>
class Fun_Lines_c : public Fun_Base_c<PIX_C, SENS_C, VARNS_C> {

    typedef Fun_Base_c<PIX_C, SENS_C, VARNS_C> parent_t;

    public:

    Fun_Lines_c(PIX_C &inp, SENS_C &insens, VARNS_C &invarns) :
        parent_t(inp,insens,invarns), count(0) {};

    void _tick() {
        uint8_t idx = parent_t::varns.var0_idx & 0x7;
        uint8_t g_phase = pgm_read_byte(g_phases + idx);
        uint8_t b_phase = pgm_read_byte(b_phases + idx);

        parent_t::pixels.setAll(0);
        for (uint8_t i=0;i<6;i++) {
            uint8_t j = count & 0x7;
            if (j > 3) j = 7-j;
            uint8_t r = sine8(count + 8*j);
            uint8_t g = sine8(count + g_phase + 8*j);
            uint8_t b = sine8(count + b_phase + 8*j);
            parent_t::pixels.set(pgm_read_byte(line_elems + j)+5*i, r,g,b);
            if (!(count % 20)) parent_t::pixels.set(2 + 5*i, parent_t::sensors.rand32());
        }
        count += 1;
    };
    private:
        uint32_t count;
};



#endif
