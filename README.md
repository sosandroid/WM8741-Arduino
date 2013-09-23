WM8741-Arduino
==============
V 1.0
An Arduino Library for Wolfson Micro WM8741 DACs using I2C bus. This has been done to manage the [Twisted Pear Audio Opus DAC](http://www.twistedpearaudio.com/digital/opus.aspx) in software mode. Any board using WM8741 will work the same way.

## Features
- Stereo & Dual mono setups
- Volume (set, up, down)
- Registers manipulation (read, write)
- Filters
- Mute & Zero sound
- Soft reset
- Check mute status

## Not supported

Does not support at this time DSD or 8fs features of the chip. To be added.

## Misc.

Should work on WM874x chips.

[WM8741 Datasheet](http://www.wolfsonmicro.com/products/dacs/WM8741/)

## Hardware
WM8741 is a 3.3V device, if your Arduino board uses a 5V I2C bus, please use a level shifter provided by any of your usual harware store
