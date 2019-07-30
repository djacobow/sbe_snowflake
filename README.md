
# Electronic Snowflake

If you are looking at this repository, it is because you are considering
reprogramming your electronic snowflake.

Of course I totally invite you to! I think it's a lot of fun. But please
note:

 *THE CODE YOU FIND HERE IS NOT THE SAME AS WHAT COMES ON THE SNOWFLAKE*

The SBE Snowflake is not an open-source project.

* On the hardware side, schematics are provided, but design files,
  artwork, and bill of materials necessary to make your own boards
  are not provided.

* On the software side, I am providing a sketch in the `firmware/`
  folder which demonstrates how to read the sensors and to drive a
  pattern onto the LEDs, but this sketch does not implement the
  functionality that comes with the Snowflake.


If you own a Snowflake and reprogram it, you will lose the original
sketch. If you would like to restore the sketch, contact me at 
dave -at- southberkeleyelectronics.com, and I will send you a 
`.hex` file you can use to reprogram your board using avrdude.

## Instructions for Arduino setup

Please find the Arduino setup instructions in
the User manual found on the resources page here:
https://www.southberkeleyelectronics.com/home/snowflake

The main bit of setup you need to do is:

* install a header to receive the serial or ICSP connector, depending on which 
  programming approach you intend to use.

* Add an entry to boards.text for a 8MHz pro mini (necessary snippet
  of `boards.txt` is included in this repo) The `boards.text` modification
  is only necessary if you use serial programming or if you intend to
  change fuse settings.


