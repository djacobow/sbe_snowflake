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


#include <Arduino.h>
#include "debug.h"
#include "helpers.h"
#include "bpress.h"
#include "sensors.h"
#include "pixchain.h"
#include "fun_stuff.h"
#include "stored.h"
#include "powerctrl.h"
#include "ir.h"

// defintions of different button press lengths
const uint16_t  SHORT_PRESS_MILLIS      = 200;
const uint16_t  MED_PRESS_MILLIS        = 1000;
const uint16_t  LONG_PRESS_MILLIS       = 8000;
const uint32_t  PATTERN_DURATION_MILLIS = 30000;

// total number of "pixels"
const uint8_t   PIXEL_CHAIN_LENGTH = 30;
const uint32_t  ALL_LIGHTS_MSK     = 0x3fffffff;

// Pin assignments
const uint8_t   PIXEL_OUTPUT_PIN   = 4;
const uint8_t   BUTTON1_PIN        = 9;
const uint8_t   BUTTON2_PIN        = 8;
const uint8_t   LIGHT_PIN          = A1;
const uint8_t   SOUND_PIN          = A0;
const uint8_t   NOISE_PIN          = A7;
const uint8_t   RF_PIN             = A6;
const uint8_t   LED_PWR_CTRL_PIN   = A2;
const uint8_t   IR_PWR_CTRL_PIN    = A3;
const uint8_t   IR_PIN             = 2;
const uint8_t   LOWVOLT_LED_PIN    = 3;

// voltage levels that define when to shut down
// (rather than drain batteries until they leak)
const uint16_t  MIN_VOLTS_MV       = 3000UL;
const uint16_t  EXTERNAL_MV_THRESH = 4800UL;
const uint8_t   MAX_LOWVOLT_ITERS  = 30;

// user-selectable loop delays between led updates
// faster than 70ms between updates and the IR remote cannot function with 
// 32b codes like the NEC codes in the cheap AliExpress remotes. For remotes 
// with 16b codes (sony), 20ms is sufficient.
const uint16_t delays[]          PROGMEM = { 70, 100, 200, 500, 1000, 5000, 5, 20, 40 };

// user selectable max brightnesses
const uint8_t  brightnesses[]    PROGMEM = { 50, 100, 150, 200, 25 };
// user selectable delay before turnoff, in increments of 5 minutes
const uint8_t on_times_5mins[]   PROGMEM = { 6, 12, 24, 36, 72, 144, 255, 1, 3 };

typedef struct varn_indices_t {
    uint8_t pattern_idx;
    uint8_t delay_idx;
    uint8_t brite_idx;
    uint8_t turnoff_idx;
    uint8_t sound_idx;
    uint8_t var0_idx;
    uint8_t auto_idx;
} varn_indices_t;

const uint8_t  VARIATION_0_COUNT = 8;

varn_indices_t varn_indices;


//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wc++11-extensions"

template<uint8_t PIN>
using Button_sc = Button_c<PIN, SHORT_PRESS_MILLIS, MED_PRESS_MILLIS, LONG_PRESS_MILLIS>;
Button_sc<BUTTON1_PIN> button1;
Button_sc<BUTTON2_PIN> button2;

//#pragma GCC diagnostic pop

typedef PixChain_c<PIXEL_CHAIN_LENGTH, PIXEL_OUTPUT_PIN> PixChain_sc;
PixChain_sc pixels;

typedef Sensors_c<LIGHT_PIN,SOUND_PIN,NOISE_PIN, RF_PIN> Sensors_sc;
Sensors_sc sensors;

void wake_handler(void);

typedef powerControl_c<LED_PWR_CTRL_PIN, IR_PWR_CTRL_PIN> powerControl_sc;
powerControl_sc pctrl(&wake_handler);

const uint8_t IR_WAIT_LOOPS = 100; // how often can a button repeat
typedef ir_c<IR_PIN,IR_WAIT_LOOPS> ir_sc;
ir_sc irdecoder;


// interrupt to be called when device is woken from sleep
void wake_handler(void) {
    sleep_disable();
    // remove the interrupt from the wake handler and let
    // the IR decoder use it again
    detachInterrupt(0);
    irdecoder.restart();
    DEBUG_PRINTLN_F("wakeable woke by interrupt");
    ADCSRA |= bit(ADEN);
    for (uint8_t pcount=0;pcount<20;pcount++) {
        ir_decoded_t d = irdecoder.code();
        // the unit can be woken only with the "power" (off) button
        if (
             (d.status == ir_decoded) &&
             (
               (d.value == ir_button_off_wakeable) ||
               (d.value == ir_button_off_hard)
             )
           ) {
            pctrl.request_reset();
            break;
        }
        delay(250);
    }
    DEBUG_PRINTLN_F("wake_handler compelete");
    DEBUG_FLUSH();
};


Fun_Sparkle_c <PixChain_sc, Sensors_sc, varn_indices_t>
    fun_sparkle(pixels, sensors, varn_indices);
Fun_Chaser_c  <PixChain_sc, Sensors_sc, varn_indices_t>
    fun_chaser(pixels, sensors, varn_indices);
Fun_Rainbow_c <PixChain_sc, Sensors_sc, varn_indices_t>
    fun_rainbow(pixels, sensors, varn_indices);
Fun_Sparse_c  <PixChain_sc, Sensors_sc, varn_indices_t>
    fun_sparse(pixels, sensors, varn_indices);
Fun_Snake_c   <PixChain_sc, Sensors_sc, varn_indices_t, 7> 
    fun_snake(pixels, sensors, varn_indices);
Fun_Leaves_c  <PixChain_sc, Sensors_sc, varn_indices_t>
    fun_leaves(pixels, sensors, varn_indices);
Fun_Flash_c   <PixChain_sc, Sensors_sc, varn_indices_t>
    fun_flash(pixels, sensors, varn_indices);
Fun_Cylon_c   <PixChain_sc, Sensors_sc, varn_indices_t>
    fun_cylon(pixels, sensors, varn_indices);
Fun_Solid_c   <PixChain_sc, Sensors_sc, varn_indices_t>
    fun_solid(pixels, sensors, varn_indices);
Fun_MiniCircle_c   <PixChain_sc, Sensors_sc, varn_indices_t>
    fun_minicircle(pixels, sensors, varn_indices);
Fun_Inching_c<PixChain_sc, Sensors_sc, varn_indices_t>
    fun_inching(pixels, sensors, varn_indices);
Fun_Pulse_c<PixChain_sc, Sensors_sc, varn_indices_t>
    fun_pulse(pixels, sensors, varn_indices);
Fun_Fade_c<PixChain_sc, Sensors_sc, varn_indices_t>
    fun_fade(pixels, sensors, varn_indices);
Fun_Lines_c<PixChain_sc, Sensors_sc, varn_indices_t>
    fun_lines(pixels, sensors, varn_indices);
Fun_Settings_c<PixChain_sc, Sensors_sc, varn_indices_t>
    fun_settings(pixels, sensors, varn_indices);

typedef Fun_Base_c    <PixChain_sc, Sensors_sc, varn_indices_t>  Fun_base_sc;

Fun_base_sc *patterns[] = { 
    &fun_chaser, 
    &fun_sparkle, 
    &fun_rainbow, 
    &fun_sparse,
    &fun_snake,
    &fun_leaves,
    &fun_flash,
    &fun_cylon,
    &fun_solid,
    &fun_minicircle,
    &fun_inching,
    &fun_pulse,
    &fun_fade,
    &fun_lines,
    &fun_settings,
};

stored2_c<varn_indices_t, 0x1> eeprom(varn_indices);


// routine to scale a limited range input to a different
// limited range
template<uint16_t in_min, uint16_t in_max>
uint8_t scale_range(uint16_t in, uint8_t out_min, uint8_t out_max) {
    const uint32_t in_range = in_max - in_min;
    const uint32_t out_range = out_max - out_min;
    uint32_t in_v     = in;
    if (in_v < in_min) in_v = in_min;
    if (in_v > in_max) in_v = in_max;
    in_v -= in_min;
    uint32_t fract = (in_v << 16) / in_range;
    uint32_t out_v = (fract * out_range) >> 16;
    out_v += out_min;
    return out_v;
};


void shutdown(pc_shutdown_mode_t shmode = pctrl_off) {
    DEBUG_PRINTLN_F("top-level shutdown");
    pixels.disable();
    pctrl.shutdown(shmode);
}

typedef enum sound_mode_t {
    SOUND_OFF,
    SOUND_VU,
    SOUND_FLASH,
} sound_mode_t;

typedef enum autochange_mode_t {
    AUTO_STOP,
    AUTO_PATTERN,
    AUTO_VARIATION,
    AUTO_PATTERN_VARIATION,
} autochange_mode_t;


uint32_t last_tick;
uint32_t last_touch;
uint32_t last_autochange;
uint8_t  lowvolt_count;
pc_shutdown_mode_t wake_status;

void lowvolt_preshutdown() {
   pctrl.poweron(false); // turn off the LED power right away
   pinMode(LOWVOLT_LED_PIN,OUTPUT);
   for (uint8_t i=0;i<30; i+=1) {
       digitalWrite(LOWVOLT_LED_PIN,LOW);
       delay(250);
       digitalWrite(LOWVOLT_LED_PIN,HIGH);
       delay(250);
   }
   pinMode(LOWVOLT_LED_PIN,INPUT);
};

void setup() {
    DEBUG_INIT(19200);

    lowvolt_count = 0;
    wake_status = pctrl_running;

    // turn on LED chain
    DEBUG_PRINTLN_F("before LED poweron");
    pctrl.poweron(true);
    delay(250);
    DEBUG_PRINTLN_F("before IR poweron");
    pctrl.alton(true);
    delay(250);

    uint8_t tries = 10;
    while (tries) {
        uint16_t volts = sensors.myVcc();
        if (volts < MIN_VOLTS_MV) {
            tries -= 1;
            if (!tries) {
                pctrl.poweron(false);
                pctrl.alton(false);
                lowvolt_preshutdown();
                shutdown();
            }
            DEBUG_PRINTLN_F("low battery?");
            DEBUG_PVAR(volts);
            delay(100);
        }  else {
            tries = 0;
        }
    }

    pixels.clear();

    DEBUG_PRINTLN_F("before EEP retreive");
    eeprom.retrieve();
    DEBUG_PRINTLN_F("after EEP retreive");

    // This is in case the eeprom has never been written
    if (varn_indices.pattern_idx  >= getLength(patterns))         varn_indices.pattern_idx   = 0;
    if (varn_indices.delay_idx    >= getLength(delays))           varn_indices.delay_idx  = 0;
    if (varn_indices.brite_idx    >= getLength(brightnesses))     varn_indices.brite_idx  = 0;
    if (varn_indices.turnoff_idx  >= getLength(on_times_5mins))   varn_indices.turnoff_idx = 0;
    if (varn_indices.var0_idx     >= VARIATION_0_COUNT)           varn_indices.var0_idx = 0;
    if (varn_indices.sound_idx    >  SOUND_FLASH)                 varn_indices.sound_idx = 0;
    if (varn_indices.auto_idx     >  AUTO_PATTERN_VARIATION)      varn_indices.auto_idx = 0;
    patterns[varn_indices.pattern_idx]->init();
    sensors.reseed();
    last_touch = last_tick = last_autochange = millis();
    DEBUG_PRINTLN_F("setup complete");
};


void loop() {

   bp_t bp1v = button1.pressed();
   bp_t bp2v = button2.pressed();
   ir_decoded_t ircode = irdecoder.code();
   uint16_t ll = sensors.light();
   uint16_t sl = sensors.sound();

   if (bp1v != bp_none) {
       last_touch = last_tick; 
       DEBUG_PVAR(bp1v);
   }
   if (bp2v != bp_none) {
       last_touch = last_tick; 
       DEBUG_PVAR(bp2v);
   }
   if (ircode.status != ir_nothing) {
       DEBUG_PVAR(ircode.status);
       DEBUG_PVAR(ircode.value);
       if (ircode.status == ir_decoded) {
           last_touch = last_tick; 
       }
   }

   uint8_t l_scaled = scale_range<0,500>(ll,10,fromProgMem8(brightnesses,varn_indices.brite_idx));

   uint32_t msk  = ALL_LIGHTS_MSK;
   switch ((sound_mode_t)varn_indices.sound_idx) {
       case SOUND_VU:
           msk = scale2mask<0,3100,PIXEL_CHAIN_LENGTH,ALL_LIGHTS_MSK>(sl);
           break;
       case SOUND_FLASH:
           msk = thresholder(sl) ? ALL_LIGHTS_MSK : 0;
           break;
       default:
           break;
   }

   auto incr_pattern = [&] () {
       wrapIncr(varn_indices.pattern_idx,getLength(patterns));
       DEBUG_PVAR(varn_indices.pattern_idx);
       patterns[varn_indices.pattern_idx]->init();
   };
   auto decr_pattern = [&] () {
       wrapDecr(varn_indices.pattern_idx,getLength(patterns));
       DEBUG_PVAR(varn_indices.pattern_idx);
       patterns[varn_indices.pattern_idx]->init();
   };
   auto incr_variation = [&] () {
       wrapIncr(varn_indices.var0_idx,VARIATION_0_COUNT);
       DEBUG_PVAR(varn_indices.var0_idx);
   };
   auto save_state = [&] () {
       eeprom.store();
   };
   auto incr_mode = [&] () {
       wrapIncr(varn_indices.sound_idx, SOUND_FLASH + 1);
   };
   auto incr_auto = [&] () {
       wrapIncr(varn_indices.auto_idx, AUTO_PATTERN_VARIATION + 1);
       DEBUG_PVAR(varn_indices.auto_idx);
   };
   auto decr_delay = [&] () {
       wrapDecr(varn_indices.delay_idx,getLength(delays));
   };
   auto incr_delay = [&] () {
       wrapIncr(varn_indices.delay_idx,getLength(delays));
   };
   auto incr_brite = [&] () {
       wrapIncr(varn_indices.brite_idx,getLength(brightnesses));
   };
   auto incr_shutdown_delay = [&] () {
       wrapIncr(varn_indices.turnoff_idx,getLength(on_times_5mins));
   };

   switch (bp1v) {
       case bp_short:    incr_pattern(); break;
       case bp_medium:   incr_variation(); break;
       case bp_long:     wake_status = pctrl_off; break;
       case bp_verylong: save_state(); break;
       default: break;
   }
   switch (bp2v) {
       case bp_short:    incr_mode(); break;
       case bp_medium:   incr_delay(); break;
       case bp_long:     incr_brite(); break;
       case bp_verylong: incr_shutdown_delay(); break;
       default: break;
   }

   if (ircode.status == ir_decoded) {
       switch (ircode.value) {
           case ir_button_next_pattern   : incr_pattern(); break;
           case ir_button_prev_pattern   : decr_pattern(); break;
           case ir_button_next_variation : incr_variation(); break;
           case ir_button_save           : save_state(); break;
           case ir_button_next_mode      : incr_mode(); break;
           case ir_button_next_speed     : incr_delay(); break;
           case ir_button_prev_speed     : decr_delay(); break;
           case ir_button_next_brite     : incr_brite(); break;
           case ir_button_off_wakeable   : wake_status = pctrl_wakeable; break;
           case ir_button_off_hard       : wake_status = pctrl_off; break;
           case ir_button_next_ontime    : incr_shutdown_delay(); break;
           case ir_button_next_auto      : incr_auto(); break;
           default: break;
       }
   }

   


   uint32_t now = millis();
   uint32_t tick_elapsed  = now - last_tick;
   uint32_t touch_elapsed = now - last_touch;
   uint16_t del = fromProgMem16(delays, varn_indices.delay_idx);
   uint32_t pat_elapsed   = now - last_autochange;

   if (varn_indices.auto_idx && (pat_elapsed > PATTERN_DURATION_MILLIS)) {
       if (varn_indices.auto_idx & 0x1) {
           varn_indices.pattern_idx = sensors.rand32() % getLength(patterns);
           patterns[varn_indices.pattern_idx]->init();
       };
       if (varn_indices.auto_idx & 0x2) 
           varn_indices.var0_idx = sensors.rand32() % VARIATION_0_COUNT;
       last_autochange = now;
       DEBUG_PVAR(last_autochange);
   }

   if (tick_elapsed > del) {

       /* logic to deal with a shutdown is in this tick fn
          because there is some strange issue where if I call
          pixels.show() anywhere other than here, the first
          pixel glows green no matter what. This is a timing issue
          of some kind and doesn't happen if there is exactly one
          call to pixels.show. Maybe a lto/inlining thing?
       */

       if (wake_status != pctrl_running) {
           pixels.clear();
       } else {
           patterns[varn_indices.pattern_idx]->tick();
       }

       pixels.copyToOut(msk, l_scaled);
       if (irdecoder.isIdle()) {
           pixels.show();
       }

       if (wake_status != pctrl_running) {
           if ((wake_status == pctrl_wakeable) && (sensors.myVcc() < EXTERNAL_MV_THRESH)) {
               DEBUG_PRINTLN_F("Forcing hard off because (probably) on battery.");
               wake_status = pctrl_off;
           }
           DEBUG_PVAR(wake_status);
           shutdown(wake_status);
       }

       last_tick = now;

   } else {
       uint16_t lv_meas = sensors.myVcc();
       if (lv_meas < MIN_VOLTS_MV) {
           DEBUG_PRINTLN_F("Detected low voltage.");
           DEBUG_PVAR(lv_meas);
           lowvolt_count += 1;
           DEBUG_PVAR(lowvolt_count);
       } else {
           lowvolt_count = 0;
       }
       if (lowvolt_count >= MAX_LOWVOLT_ITERS) {
           lowvolt_preshutdown();
           shutdown();
       }
       // there is a 2ms delay inside the sensor read voltage routine
       // delay(2);
   }

   uint32_t turn_off_millis = 
       (uint32_t)fromProgMem8(on_times_5mins,varn_indices.turnoff_idx) * 5UL * 60UL * 1000UL;
   if (touch_elapsed > turn_off_millis) {
       DEBUG_PRINTLN_F("Setting to wakeable shutdwon.");
       wake_status = pctrl_wakeable;
   }
};

