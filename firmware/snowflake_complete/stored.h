#ifndef __stored_h
#define __stored_h

#include <EEPROM.h>


template<typename T, int EEP_BASE_ADDR>
class stored2_c {

    public:
        stored2_c(T &it) :t(it) {};

        void retrieve() {
            uint8_t *uip = (uint8_t *)(void *)&t;

            for (uint8_t j=0;j<sizeof(T);j++) {
                int full_addr = EEP_BASE_ADDR + j;
                uint8_t d = EEPROM.read(full_addr);
                uip[j] = d;
            }
        };
 
        void store() {
            uint8_t *uip = (uint8_t *)(void *)&t;

            for(uint8_t j=0; j<sizeof(T);j++) {
                int full_addr = EEP_BASE_ADDR + j;
                uint8_t d = uip[j];
                EEPROM.write(full_addr,d);
            }
        };

    private:
        T &t;
};

#endif

