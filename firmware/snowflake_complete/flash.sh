
do_bootloader=1

if [ $do_bootloader ]
then
    avrdude \
        -v \
        -B 5 \
        -p m328p \
        -c usbtiny \
        -U lock:w:0x3f:m \
        -U efuse:w:0xff:m \
        -U lfuse:w:0xe2:m \
        -U hfuse:w:0xd8:m \

    avrdude \
        -v \
        -B 5 \
        -p m328p \
        -c usbtiny \
        -U flash:w:./snowflake_complete.ino.with_bootloader.eightanaloginputs.hex:i \
        -U lfuse:w:0xe2:m \
        -U hfuse:w:0xd8:m \
        -U efuse:w:0xff:m \
        -U lock:w:0x0f:m \

else
    avrdude \
        -p m328p \
        -c usbtiny \
        -U lfuse:w:0xe2:m \ # set to 8 MHz
        -U snowflake_complete.ino.hex
fi

