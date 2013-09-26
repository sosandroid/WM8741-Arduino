/**********************************************************************
* Example for managing the WM8741 device using I2C and Arduino Sketch
*
* Arduino IDE 1.0
* Tested on Arduino Mega 2650
* 
* Requires Wire library
*
* 12/08/2012
**********************************************************************/

#include <WM8741.h>
//dac set in stereo mode
//attenuation nim : 0, attenuation max : 99, attenuation current : 60
WM8741 opusdac(false, 0x00, 0x63, 0x3C);

void setup() {
  //do anything here
  opusdac.SetFilter(0x01);
}

void loop() {

  // loop filters and change volume every 15 seconds
  opusdac.NextFilter();
  opusdac.SetVolume(0x32); //attenuation level 50
  delay(15000);
  opusdac.NextFilter();
  opusdac.SetVolume(0x28);  //attenuation level 40
  delay(15000);
}
