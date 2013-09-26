/*
Arduino library for Wolfson Microelectronics WM8741 using I2C bus.
Done to manage PCM data using I2S bus
Do not manage DSD or 8FS 

Version 1.0
Aout 2012

Tested on Arduino Mega 2560
Uses Arduino 1.0.1 IDE

/!\ Use an I2C level converter as WM8741 supports 3.3v only on the bus - example part http://dsscircuits.com/i2c-level-converter.html
*/



#ifndef WM8741_H
#define WM8741_H
     
#include <Arduino.h>
#include <wire.h>

/*
WM8741 Class
*/

class WM8741 {
    public:
            WM8741(bool monomode, char unsigned VolMax, char unsigned VolMin, char unsigned VolCurr);
            void SetupStereoMode();
	    void SetupMonoMode();
	    void setReg (uint8_t ChipAddress, uint8_t RegAddress, uint8_t RegValue);
	    void SetVolume(int unsigned volume, bool changeMute);
	    void SetFilter(int filter);
	    void NextFilter();
	    void PrevFilter();
	    void VolumeUp();
	    void VolumeDown();
	    void Mute();
	    void ZeroSound();
	    void SoftReset();
	    void getReg(uint8_t ChipAddress, uint8_t RegAddress);
	    void CheckMuteStatus();
	    //Todo : add mute status feedback, current volume feedback
};
     
#endif
