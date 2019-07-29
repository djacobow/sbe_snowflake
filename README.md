
# Electronic Snowflake

If you are looking at this repository, it is because you are considering reprogramming your electronic snowflake.

Of course I totally invite you to, but be warned:

 *THE CODE YOU FIND HERE IS NOT THE SAME AS WHAT COMES ON THE SNOWFLAKE*

The snowflake is not an open-source project. I am providing the schematics for the hardware, but not the artwork
necessary to make your own boards.

Similarly, for the firmware, I am providing a sketch below that demonstrates how the snowflake works, but
does not include most of the patterns or features of the SBE Electronic Snowflake.

If you own a Snowflake and reprogram and and want to get the original sketch back on the board, please
email me at dave -at- southberkeleyelectronics.com, and I will send you a .hex file that you can use to
rezap the board using avrdude.

## Instructions for Arduino setup

Please find the Arduino setup instructions in the User manual found on the resources
page here: https://www.southberkeleyelectronics.com/home/snowflake

The main bit of setup you need to do is:

* install a header to receive the serial or ICSP connector, depending on which 
  programming approach you intend to use.

* Add an entry to boards.text for a 8MHz pro mini (necessary snipped of `boards.txt` is included in this repo)
  The `boards.text` modification is only necessary if you use serial programming or if you intend to change
  fuse settings


