WM8741-Arduino
==============
V 1.0

An Arduino Library for Wolfson Micro [WM8741](http://www.cirrus.com/en/products/wm8741.html) DACs using I2C bus. This has been done to manage the [Twisted Pear Audio Opus DAC](http://www.twistedpearaudio.com/digital/opus.aspx) in software mode. Any board using WM8741 will work the same way.

## Features ##
- Stereo & Dual mono setups
- Volume (set, up, down)
- Registers manipulation (read, write)
- Filters
- Mute & Zero sound
- Soft reset
- Check mute status

## Todo / Not supported ##

Does not support at this time DSD or 8fs features of the chip. To be added. 

Move to [I2Cdev](https://github.com/jrowberg/i2cdevlib) to add more structure and address all registers


## Misc. ##

Should also work on WM8742 chips.

[WM8741 Datasheet](http://www.wolfsonmicro.com/documents/uploads/data_sheets/en/WM8741.pdf)

## Hardware ##
WM8741 is a 3.3V device, if your Arduino board uses a 5V I2C bus, please use a level shifter provided by any of your usual hardware provider

## Usage ##
Copy the library into your Arduino IDE Library folder. Here one example sketch

		#include <WM8741.h>
			//dac set in stereo mode
			//attenuation nim : 0dB, attenuation max : -127dB, attenuation current : -50dB
		WM8741 opusdac(false, 0x00, 0x3FF, 0x190);

		void setup() {
				//do anything here
				opusdac.SetFilter(0x01);
				opusdac.SetVolume(0x190, true);
		}

		void loop() {
				// loop filters and change volume every 15 seconds
				opusdac.NextFilter();
				opusdac.SetVolume(0x190); //attenuation level -50
				delay(15000);
				opusdac.NextFilter();
				opusdac.SetVolume(0xC8);  //attenuation level -25
				delay(15000);
		}
