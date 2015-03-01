/**********************************************************************
* Example for managing the WM8741 device using I2C and Arduino Sketch
*
* Arduino IDE 1.0
* Tested on Arduino Mega 2650
* 
* Requires Wire library
*
* 12/08/2012
* V1.0
**********************************************************************/

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
