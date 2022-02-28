////////////////////////////////////////////////////////////////
// 
// Copyright 2019 South Berkeley Electronics
//
// Sample *minimalist* Arduino sketch to control an South Berkeley
// Electronics snowflake.
//
// *** *** *** 
// This is NOT the same sketch that comes on an SBE Electronic
// Snowflake. This is a cut-down version intended to show how to 
// program the device.
// *** *** *** 
//
// Author: Dave Jacobowitz
// (dave@southberkeleyelectronics.com)
//
////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include "pixchain.h"
#include "effects.h"
#include "ir.h"

const uint8_t   PIXEL_CHAIN_LENGTH = 30;
const uint32_t  ALL_ON_MSK         = 0x3fffffff;

// Pin assignments
const uint8_t   PIXEL_OUTPUT_PIN   = 4;
const uint8_t   BUTTON1_PIN        = 9;
const uint8_t   BUTTON2_PIN        = 8;
const uint8_t   LIGHT_PIN          = A1;
const uint8_t   SOUND_PIN          = A0;
const uint8_t   NOISE_PIN          = A7;
const uint8_t   LED_PWR_CTRL_PIN   = A2;
const uint8_t   IR_PWR_CTRL_PIN    = A3;
const uint8_t   IR_PIN             = 2;
const uint8_t   LOWVOLT_LED_PIN    = 3;

// this "pixels" object contains the the routines to drive the
// color data onto the LEDs. See pixchain.h for details.
typedef PixChain_c<PIXEL_CHAIN_LENGTH, PIXEL_OUTPUT_PIN> PixChain_sc;
PixChain_sc pixels;

const uint8_t IR_WAIT_LOOPS = 100; // how often can a button repeat
typedef ir_c<IR_PIN,IR_WAIT_LOOPS> ir_sc;
ir_sc irdecoder;

// our effect
Effect_Sparkle_c <PixChain_sc> sparkler(pixels);


// helper function for turning on power to LEDs and
// audio circuit. If you are planning on running on 
// battery power, it's important that you power down
// these circuits when idle, even if the LEDs are off,
// because they and the audio circuit draw current
// even when "off"
void setLEDPower(bool on) {
    if (on) {
        pinMode(LED_PWR_CTRL_PIN,OUTPUT);
        digitalWrite(LED_PWR_CTRL_PIN,LOW);
    } else {
        digitalWrite(LED_PWR_CTRL_PIN,HIGH);
        pinMode(LED_PWR_CTRL_PIN,INPUT_PULLUP);
    }
}

// helper function for turning off power to the IR
// receiver. The IR receiver draws a small amount of
// power at all times. You might leave this off to 
// extend battery life or on if you want to be able
// to use the IR transmitter to wake the unit.
void setIRPower(bool on) {
    if (on) {
        pinMode(IR_PWR_CTRL_PIN,OUTPUT);
        digitalWrite(IR_PWR_CTRL_PIN,HIGH);
    } else {
        pinMode(IR_PWR_CTRL_PIN,INPUT);
        digitalWrite(IR_PWR_CTRL_PIN,LOW);
    }
}


void setup() {
    Serial.begin(19200);

    // configure the buttons. There are no pullups on the board; use
    // the internal pullups in the Atmega
    pinMode(BUTTON1_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);

    // configure the light sensor
    pinMode(LIGHT_PIN,INPUT);

    // configure the sound sensor
    pinMode(SOUND_PIN,INPUT);

    // turn on LED chain
    setLEDPower(true);
    delay(250); // it's good to let power circuits settle a moment

    // turn on IR sensor
    setIRPower(true);
    delay(250); // it's good to let power circuits settle a moment

    // clear all the pixels to "off"
    pixels.clear();

    // initialize the effect
    sparkler.init();

    Serial.println(F("setup complete"));
};


void loop() {

   // buttons read low when pressed
   bool     btn1          = digitalRead(BUTTON1_PIN);
   bool     btn2          = digitalRead(BUTTON2_PIN);

   // light sensor typical range 0 (darkness) to 900-100 (sun)
   uint16_t ambient_light = analogRead(LIGHT_PIN);    

   // sound sensor typical range 0 (silence)  to ~400 (loud yell)
   uint16_t loudness      = analogRead(SOUND_PIN);

   // library configured only to display NEC codes right now,
   // everything else is an error
   ir_decoded_t ircode    = irdecoder.code();

   // print everything out
   Serial.print(F("ambient_light(")); Serial.print(ambient_light);    Serial.print(F("), ")); 
   Serial.print(F("loudness("));      Serial.print(loudness);         Serial.print(F("), ")); 
   Serial.print(F("button1("));       Serial.print(btn1);             Serial.print(F("), ")); 
   Serial.print(F("button2("));       Serial.print(btn2);             Serial.print(F("), ")); 
   Serial.print(F("irstatus("));      Serial.print(ircode.status);    Serial.print(F("), ")); 
   Serial.print(F("irvalue(0x"));     Serial.print(ircode.value,HEX); Serial.print(F("), ")); 
   Serial.println();

   // increment the effect/animation. This causes the working buffer in
   // the pixels object to be changed.
   sparkler.tick();

   // copy the working buffer of the pixel chain to the output buffer, 
   // respecting a bitmask for LEDs and a byte value for brightness
   // adjustment. The brightness is demonstrated here, but the mask is
   // fixed.
   pixels.copyToOut(ALL_ON_MSK, map(ambient_light,0,1000,0,255));

   if (irdecoder.isIdle()) {
       // write the output buffer to the LEDs
       pixels.show();
   }

   delay(100);
};

