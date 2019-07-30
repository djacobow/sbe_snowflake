
do_bootloader=1

if [ $do_bootloader ]
then
    avrdude \
        -p m328p \
        -c usbtiny \
        -U lock:w:0x3f:m \
        -U efuse:w:0xff:m \
        -U lfuse:w:0xe2:m \
        -U hfuse:w:0xd8:m \


    avrdude \
        -p m328p \
        -c usbtiny \
        -U flash:w:./sbe_snowflake.ino.with_bootloader.eightanaloginputs.hex:i \
        -U lfuse:w:0xe2:m \
        -U hfuse:w:0xd8:m \
        -U efuse:w:0xff:m \
        -U lock:w:0x0f:m \

else
    avrdude \
        -p m328p \
        -c usbtiny \
        -U lfuse:w:0xe2:m \ # set to 8 MHz
        -U sbe_snowflake.ino.hex
fi

