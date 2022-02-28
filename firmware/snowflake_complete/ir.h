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

#ifndef __IR_H
#define __IR_H

#include <stdint.h>
#include <IRLib2.h>

typedef enum ir_decode_status_e {
    ir_nothing,
    ir_decoded,
    ir_error,
} ir_decode_status_e;


typedef enum ir_button_e {
    ir_button_invalid = 0,
    ir_button_next_pattern,
    ir_button_next_variation,
    ir_button_save,
    ir_button_next_mode,
    ir_button_next_speed,
    ir_button_next_brite,
    ir_button_off_wakeable,
    ir_button_off_hard,
    ir_button_next_ontime,
    ir_button_prev_speed,
    ir_button_prev_pattern,
    ir_button_next_auto,
} ir_button_e;

typedef struct ir_decoded_t {
    ir_decode_status_e status;
    ir_button_e value;
} ir_decoded_t;


template<uint8_t PIN, uint8_t wait_time>
class ir_c {
    public:
        ir_c() : recv(PIN), wait_count(0) {
            recv.enableIRIn();
        };
        void restart() {
            recv.disableIRIn();
            recv.enableIRIn();
        }
        ir_decoded_t code() {
            ir_decoded_t rv = {ir_nothing, ir_button_invalid };
            if (recv.getResults()) {
                decoder.decode();
                if ((decoder.protocolNum == SONY) && (decoder.bits == 12)) {
                    rv = decodeSony();
                } else if ((decoder.protocolNum == NEC) && (decoder.bits == 32)) {
                    rv = decodeNEC();
                } else {
                    rv.status = ir_error;
                }
                if (rv.status == ir_error) {
                    decoder.dumpResults();
                }
                recv.enableIRIn();

                if (rv.status == ir_decoded) {
                    if (wait_count && (rv.value == last_returned.value)) {
                        rv.status = ir_nothing;
                    }  else {
                        last_returned = rv;
                    }
                }
            }
            if (wait_count) wait_count -= 1;
            return rv;
        };
        // routine to tell us if a decode is in progress. Notice
        // that these variables are not in this class; they are 
        // globals created by the IRLib2 library. :-(
        bool isIdle() {
            return (recvGlobal.currentState == STATE_FINISHED) ||
                   (recvGlobal.currentState == STATE_READY_TO_BEGIN);
};
    private:
        ir_decoded_t decodeNEC() {
            ir_decoded_t rv = {ir_error, ir_button_invalid };
            if ((decoder.protocolNum == NEC) && (decoder.bits == 32)) {
                DEBUG_PVAR_FMT(decoder.value,HEX);
                switch (decoder.value & 0xffff) {
                    case 0xa25d: // "1"
                        rv.value = ir_button_next_pattern; 
                        break;
                    case 0x629d: // "2"
                        rv.value = ir_button_next_variation;
                        break;
                    case 0xe21d: // "3"
                        rv.value = ir_button_save;
                        break;
                    case 0x22dd: // "4"
                        rv.value = ir_button_next_mode;
                        break;
                    case 0x02fd: // "5"
                        rv.value = ir_button_next_speed;
                        break;
                    case 0xc23d: // "6"
                        rv.value = ir_button_next_brite;
                        break;
                    case 0xe01f: // "7"
                        rv.value = ir_button_off_wakeable;
                        break;
                    case 0xa857: // "8"
                        rv.value = ir_button_off_hard;
                        break;
                    case 0x906f: // "9"
                        rv.value = ir_button_next_ontime;
                        break;
                    case 0x38c7: // "OK"
                        rv.value = ir_button_save; 
                        break; 
                    case 0x6897: // "*"
                        rv.value = ir_button_off_wakeable;
                        break;
                    case 0xb04f: // "#"
                        rv.value = ir_button_next_ontime;
                        break;
                    case 0x4ab5: // "down arrow"
                        rv.value = ir_button_next_speed;
                        break;
                    case 0x18e7: // "up arrow"
                        rv.value = ir_button_prev_speed;
                        break;
                    case 0x5aa5: // "right arrow"
                        rv.value = ir_button_next_pattern;
                        break;
                    case 0x10ef: // "left arrow"
                        rv.value = ir_button_prev_pattern;
                        break;
                    case 0x9867: // "0"
                         rv.value = ir_button_next_auto;
                         break;
                    default: break;
                }
                if (rv.value >= 0) rv.status = ir_decoded;
            }
            DEBUG_PVAR(rv.status);
            return rv;
        };
        ir_decoded_t decodeSony() {
            ir_decoded_t rv = {ir_error, ir_button_invalid };
            if ((decoder.protocolNum == SONY) && (decoder.bits == 12)) {
                rv.status = ir_decoded;
                switch (decoder.value) {
                    case 0x010 : /* 1 */ rv.value = ir_button_next_pattern;   break;
                    case 0x810 : /* 2 */ rv.value = ir_button_next_variation; break;
                    case 0x410 : /* 3 */ rv.value = ir_button_save;           break;
                    case 0xc10 : /* 4 */ rv.value = ir_button_next_mode;      break;
                    case 0x210 : /* 5 */ rv.value = ir_button_next_speed;     break;
                    case 0xa10 : /* 6 */ rv.value = ir_button_next_brite;     break;
                    case 0x610 : /* 7 */ rv.value = ir_button_off_wakeable;   break;
                    case 0xe10 : /* 8 */ rv.value = ir_button_off_hard;       break;
                    case 0x110 : /* 9 */ rv.value = ir_button_next_ontime;    break;
                    // case 0x910 : /* 0 */ rv.value = 0;                        break;
                    default: break;
                }
                if (rv.value >= 0) rv.status = ir_decoded;
            }
            return rv;
        }
        IRrecv recv;
        IRdecode decoder;
        ir_decoded_t last_returned;
        uint8_t wait_count;
};

#endif

