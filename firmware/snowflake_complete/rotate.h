
#ifndef __rotator_h
#define __rotator_h

// helpers for rotation
typedef enum rotate_type_t {
    ROTATE_INNER,
    ROTATE_OUTER,
    ROTATE_ALL,
} rotate_type_t;


template<class PIXELS_C>
class rotate_base_c {
    public:
    rotate_base_c(PIXELS_C &inpixels) : pixels(inpixels) {};
    virtual uint8_t maxl() = 0;
    virtual uint8_t idx_xform(uint8_t i) = 0;

    void rotate(uint8_t ct = 1, bool dir = false) {

        uint8_t l = maxl();

        for (uint8_t i=0;i<ct;i++) {
            uint8_t last_preidx = dir ? 0 : l-1;
            pixel_t temp = pixels.get(idx_xform(last_preidx));
            for (uint8_t j=1;j<l;j++) {
                uint8_t preidx = dir ? j : l-j-1;
                uint8_t idx  = idx_xform(preidx);
                uint8_t idxp = idx_xform(dir ? preidx - 1 : preidx + 1);
                // I have an off by one I need to fix. For the time 
                // being this works.
                if (idx < pixels.len()) {
                    pixels.set(idxp,pixels.get(idx));
                    last_preidx = preidx;
                }
            }
            pixels.set(idx_xform(last_preidx),temp);
        }
    }
    protected:
        PIXELS_C &pixels;
};

template<class PIXELS_C>
class rotate_inner_c : public rotate_base_c<PIXELS_C> {
    typedef rotate_base_c<PIXELS_C> parent_t;                                 
    public:
        rotate_inner_c(PIXELS_C &inp) : parent_t(inp) {};
        uint8_t maxl() { return (2*parent_t::pixels.len()/5)+1; }
        uint8_t idx_xform(uint8_t i) { return 5 * (i/2) + (i%2) - 1; };
};
template<class PIXELS_C>
class rotate_outer_c : public rotate_base_c<PIXELS_C> {
    typedef rotate_base_c<PIXELS_C> parent_t;                                 
    public:
        rotate_outer_c(PIXELS_C &inp) : parent_t(inp) {};
        uint8_t maxl() { return (3*parent_t::pixels.len()/5); };
        uint8_t idx_xform(uint8_t i) { return 5 * (i/3) + (i%3) + 1; };
};
template<class PIXELS_C>
class rotate_all_c : public rotate_base_c<PIXELS_C> {
    typedef rotate_base_c<PIXELS_C> parent_t;                                 
    public:
        rotate_all_c(PIXELS_C &inp) : parent_t(inp) {};
        uint8_t maxl() { return parent_t::pixels.len(); };
        uint8_t idx_xform(uint8_t i) { return i; }
};


#endif

