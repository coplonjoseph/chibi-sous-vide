# chibi-sous-vide

This repository contains the firwmare for an automated sous vide machine.

## Hardware ##
- NUCLEO-F401RE
- PowerSwitch Tail II Relay
- DS18B20 temperature sensor
- An analog crock pot, rice cooker or similar

## Build Requirements ##

- GCC
- arm-non-eabi-gcc
- make

## Building ##

```bash
git submodule update --init --recursive
cd src/
make
```

Flash using OpenOCD with st_nucleo_f4.cfg
