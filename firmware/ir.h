#ifndef __IR_H
#define __IR_H

#include <stdint.h>
#include <IRLib2.h>

typedef enum ir_decode_status_e {
    ir_nothing,
    ir_decoded,
    ir_error,
} ir_decode_status_e;


typedef struct ir_decoded_t {
    ir_decode_status_e status;
    uint32_t           value;
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
            ir_decoded_t rv = {ir_nothing, 0 };
            if (recv.getResults()) {
                decoder.decode();
                if ((decoder.protocolNum == NEC) && (decoder.bits == 32)) {
                    rv.value = decoder.value;
                    if (rv.value > 0) rv.status = ir_decoded;
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
        /*
        The IR receiver uses an interrupt handler to detect transitions on
        the IR pin. Many interrupts are required to decode one infrared
        command.  However, once updating the LEDs has commenced, the
        process cannot be interrupted, it helps to skip writing the
        LEDs if an IR pattern is in the process of being received.
        This function helps detect when an IR pulse train is being
        decoded, so that LED updates can be inhibited, greatly increasing
        the chance of successfully decoding the IR message.

        You'll also note that this references variables not defined
        in this class. They are globals in the IRLib2 library.
        */
        bool isIdle() {
            return (recvGlobal.currentState == STATE_FINISHED) ||
                   (recvGlobal.currentState == STATE_READY_TO_BEGIN);
};

        
    private:
        IRrecv recv;
        IRdecode decoder;
        ir_decoded_t last_returned;
        uint8_t wait_count;
};

#endif

