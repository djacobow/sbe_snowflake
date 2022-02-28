
# Electronic Snowflake

South Berkeley Electronics Snowflake

This is the public repo for the
[South Berkeley Electronics Snowflake](https://www.southberkeleyelectronics.com/home/snowflake).

SBE has chosen to discontinue this item and instead make the design
available publically for people who would like to build their
own.

The design is licensed under the
[Create Commons Attribution - ShareAlike 3.0 license](https://creativecommons.org/licenses/by-sa/3.0/), a copy of which is included in this repository.

## What you will find here

In this repo, you should find:

### Hardware

* DipTrace files for the design
* gerbers, exported from Diptrace, if you just went to send them to a PCB house
  for manufacture. The design uses a simple 2-layer board that can be manufactured
  with the basic offerings from PCBWay, JLCPCB and others.
* A complete BOM that you can use to order parts

I cannot provide detailed guidance on how to order and complete a PCB.
There is nothing particularly difficult about this design, but in order to
complete it you should be able to solder a QFP chips, as well as 0805 size
passives. Also, the WS2812C LEDs that the design is built around are quite
sensitive to heat, and not particularly easy to solder without damage.

### Software

* There are two versions of the firmware included. A basic one
  pretty much shows that you can control the LEDs, but does not
  use all the peripherals (the IR controller, light sensor, etc.

* The complete firmware that has *all* the light modes and features
  enabled.

Both should be easily built under recent version of the Arduino
platfor, targeting the "Pro Mini 8MHz". You may have to add this
to you board.config.

## Instructions for Arduino setup

Please find the Arduino setup instructions in
the User manual found on the resources page here:
https://www.southberkeleyelectronics.com/home/snowflake

The main bit of setup you need to do is:

* install a header on your board to receive the serial or ICSP connector,
  depending on which programming approach you intend to use.

* Add an entry to boards.text for a 8MHz pro mini (necessary snippet
  of `boards.txt` is included in this repo) The `boards.text` modification
  is only necessary if you use serial programming or if you intend to
  change fuse settings.

You can program from the Arduino environment, but I usually do "export
binary" to compiler an image, and then use a script called `flash.sh` to
program my devices using AVRdude and a USBtiny.

