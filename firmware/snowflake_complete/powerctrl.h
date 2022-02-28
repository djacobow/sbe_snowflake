#ifndef __POWERCTRL_H
#define __POWERCTRL_H

#include <avr/sleep.h>
#include <avr/power.h>

typedef enum pc_shutdown_mode_t {
    pctrl_running,
    pctrl_pretend,
    pctrl_wakeable,
    pctrl_off,
} pc_shutdown_mode_t;


// willl cause a full reset on AVR
void (*resetFunc) (void) = 0;

typedef void (*wake_int_t)(void);

template<uint8_t MAIN_POWER_PIN, uint8_t ALT_POWER_PIN>
class powerControl_c {
    public:
        powerControl_c(wake_int_t iwi) : reset_requested(false), wi(iwi) { };
        void poweron(bool y) {
            _pctrl(y, MAIN_POWER_PIN, true);
        }
        void alton(bool y) {
            _pctrl(y, ALT_POWER_PIN, false);
        }
        void request_reset() {
            reset_requested = true;
        }

        void shutdown(pc_shutdown_mode_t mode = pctrl_off) {
            DEBUG_PRINTLN_F("shutdown");
            DEBUG_PVAR(mode);
            delay(100);

            if (mode == pctrl_running) return;

            switch (mode) {
                case pctrl_off:
                    poweron(false);
                    alton(false);
                    break;
                case pctrl_wakeable:
                case pctrl_pretend:
                    poweron(false);
                    alton(true);
                    break;
                default:
                    break;
            }

            // http://www.gammon.com.au/power

            // minimize our own power draw
            if ((mode == pctrl_off) || (mode == pctrl_wakeable)) {
                ADCSRA = 0;
                set_sleep_mode(SLEEP_MODE_PWR_DOWN);
                sleep_enable();
            }

            noInterrupts();

            if (mode == pctrl_wakeable) {
                // first 0 is the interrupt, the second is the ptr to the 
                // wake routine, which should trigger a hard reset (I hope!)
                attachInterrupt(0, wi, FALLING);
                EIFR = bit(INTF0);
            }

            if (mode == pctrl_off) {
                // turn stuff of everything we can turn off
                power_all_disable();
            }

            // no brown out detection:
            MCUCR = bit(BODS) | bit(BODSE);
            MCUCR = bit(BODS);

            if ((mode == pctrl_wakeable) || (mode == pctrl_pretend)) {
                interrupts();
            }

            // sleep:
            if ((mode == pctrl_wakeable) || (mode == pctrl_off)) {
                sleep_cpu();
                sleep_disable();
                DEBUG_PRINTLN_F("back from sleep");
                if (reset_requested) {
                    DEBUG_PRINTLN_F("will reset to fully reawaken");
                    DEBUG_FLUSH();
                    resetFunc();
                } else {
                    DEBUG_PRINTLN_F("no reset requested, returning to loop()");
                }
            }
        }

    private:
        void _pctrl(bool yeah, uint8_t pin, bool invert = true) {
            if (yeah) {
                pinMode(pin, OUTPUT);
                digitalWrite(pin, invert ? LOW : HIGH);
            } else {
                digitalWrite(pin ,invert ? HIGH : LOW);
                pinMode(pin,invert ? INPUT_PULLUP : INPUT);
            }
        }
        bool reset_requested;
        wake_int_t wi;
};

#endif

