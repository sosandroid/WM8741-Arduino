////////////////////////////////////////////////////////
//
// WM8741 control through I2C
// Version 1.0 Aug 2012
// Version 1.0.1 March 2015
//
// Functions
// - Setting mono / stereo
// - Volume management
// - Mute management
// - filter management
// - registers setting / reading
//
// Uses Wire library from Arduino
//
// As default, the volume is set to default during chips setup but volume is muted. It's the role of the main sketch to unmute at the right time
////////////////////////////////////////////////////////

  #include <Wire.h>
  #include "Arduino.h"
  
/* 
I2C addresses and registers
*/


const unsigned char wm8741_i2c_one = 0x1A; //wm8741 #1 I2C address - the one used in stereo mode, right in dual mono mode
const unsigned char wm8741_i2c_two = 0x1B; //wm8741 #2 I2C address - not used in stereo mode, left in dual mono mode
// see WM8741 datasheet to know how to set addresses


/*
Register addresses - Warning, this is 7bits addresses without R/W bit. Check what is exactly transmited by the Wire.h lib.
*/
const unsigned char wm8741_reg0_addr = 0x00;
const unsigned char wm8741_reg1_addr = 0x01;
const unsigned char wm8741_reg2_addr = 0x02;
const unsigned char wm8741_reg3_addr = 0x03;
const unsigned char wm8741_reg4_addr = 0x04;
const unsigned char wm8741_reg5_addr = 0x05;
const unsigned char wm8741_reg6_addr = 0x06;
const unsigned char wm8741_reg7_addr = 0x07;
const unsigned char wm8741_reg8_addr = 0x08;
const unsigned char wm8741_reg9_addr = 0x09;
const unsigned char wm8741_reg32_addr = 0x20;

/*
Init values for WM8741
*/

const unsigned char wm8741_reg0_default = 0x1F; //volume attenuation LSB left - 127.5db
const unsigned char wm8741_reg1_default = 0x1F; //volume attenuation MSB left - 127.5db
const unsigned char wm8741_reg2_default = 0x1F; //volume attenuation LSB Right - 127.5db
const unsigned char wm8741_reg3_default = 0x3F; //volume attenuation MSB Right - 127.5db + write instruction reg0-3
const unsigned char wm8741_reg4_default = 0x19; //volume control : ramp-up, Anti clipping off, Attuenation Right = right, Soft Mute enable, Infinte zero detect enabled, zero flag left AND right
const unsigned char wm8741_reg5_default = 0x0A; //Format control : 24bits, I2S, Normal polarity, BCLK normal polarity, Output phase normal, Power normal mode
const unsigned char wm8741_reg6_default = 0x00; //Filter control : default chip conf
const unsigned char wm8741_reg7_default = 0x40; //Mode control 1 : PCM, Sampling auto detect, OSR High, Mode 8x off
const unsigned char wm8741_reg8_st_default = 0x00; //Mode control 2 : Dither off, Stereo, Daisy chain audio off, daisy chain control off, dsd gain low
const unsigned char wm8741_reg8_ml_default = 0x04; //Mode control 2 : Dither off, Dual mono left, Daisy chain audio off, daisy chain control off, dsd gain low
const unsigned char wm8741_reg8_mr_default = 0x0C; //Mode control 2 : Dither off, Dual mono right, Daisy chain audio off, daisy chain control off, dsd gain low
const unsigned char wm8741_reg9_default = 0x00; //Do a soft reset
const unsigned char wm8741_reg32_default = 0x00; //Additionnal control - default chip conf

/*
Autres constantes
*/
const unsigned char wm8741_VolDef = 0x190; //-50 dB default volume
const unsigned char wm8741_VolMin = 0x3FF; // -127dB 
const unsigned char wm8741_VolMax = 0x00; // -0dB - zero attenuation

/*
Variables
*/
bool isMono = false;
bool muted = false;
byte reg4value = wm8741_reg4_default;
int unsigned CurrentVolume = 0;
int unsigned MaxVolume = 0;
int unsigned MinVolume = 0;
int unsigned FilterActive = 1; //value of the reg6 set ahead
  
////////////////////////////////////////////////////////
//Constructor of WM8741 class
////////////////////////////////////////////////////////

WM8741::WM8741(bool monomode, char unsigned VolMax = 0x400, char unsigned VolMin = 0x400 , char unsigned VolCurr = 0x400) 
{
	
	delay(2000); //wait for WM8741 to fully start up
	Wire.begin(); //Init I2C bus
	
	//Input varaiables
	if (VolMax == 0x400) {
		MaxVolume = wm8741_VolMax;
	}
	else {
		MaxVolume = VolMax;
	}
	if (VolMin == 0x400) {
		MinVolume = wm8741_VolMin;
	}
	else {
		MinVolume = VolMin;
	}
	if (VolCurr == 0x400) {
		CurrentVolume = wm8741_VolDef;
	}
	else {
		CurrentVolume = VolCurr;
	}

	//set the devices
	if (monomode) {
		SetupMonoMode();
	}
	else {
		SetupStereoMode();
	}

	//Set variables
	CheckMuteStatus(); //read value from device reg4
	
	//Set default volume
	SetVolume(CurrentVolume, false);
}

///////////////////////////////////////////////
// Set two DAC chip to differential mono mode
///////////////////////////////////////////////
void WM8741::SetupMonoMode(void) {
	isMono = true;
	softReset();
	
	//Chip One - i2C address one
	setReg (wm8741_i2c_one, wm8741_reg0_addr, wm8741_reg0_default);
	setReg (wm8741_i2c_one, wm8741_reg1_addr, wm8741_reg1_default);
	setReg (wm8741_i2c_one, wm8741_reg2_addr, wm8741_reg2_default);
	setReg (wm8741_i2c_one, wm8741_reg3_addr, wm8741_reg3_default);
	setReg (wm8741_i2c_one, wm8741_reg4_addr, wm8741_reg4_default);
	setReg (wm8741_i2c_one, wm8741_reg5_addr, wm8741_reg5_default);
	setReg (wm8741_i2c_one, wm8741_reg6_addr, wm8741_reg6_default);
	setReg (wm8741_i2c_one, wm8741_reg7_addr, wm8741_reg7_default);
	setReg (wm8741_i2c_one, wm8741_reg8_addr, wm8741_reg8_mr_default);
	//Chip two - i2C address two
	setReg (wm8741_i2c_two, wm8741_reg0_addr, wm8741_reg0_default);
	setReg (wm8741_i2c_two, wm8741_reg1_addr, wm8741_reg1_default);
	setReg (wm8741_i2c_two, wm8741_reg2_addr, wm8741_reg2_default);
	setReg (wm8741_i2c_two, wm8741_reg3_addr, wm8741_reg3_default);
	setReg (wm8741_i2c_two, wm8741_reg4_addr, wm8741_reg4_default);
	setReg (wm8741_i2c_two, wm8741_reg5_addr, wm8741_reg5_default);
	setReg (wm8741_i2c_two, wm8741_reg6_addr, wm8741_reg6_default);
	setReg (wm8741_i2c_two, wm8741_reg7_addr, wm8741_reg7_default);
	setReg (wm8741_i2c_two, wm8741_reg8_addr, wm8741_reg8_ml_default);

}

///////////////////////////////////////////////
// Set one DAC chip to Stereo mode
///////////////////////////////////////////////
void WM8741::SetupStereoMode(void) {
	isMono = false;
	softReset();
	
	//Chip One - i2C address one
	setReg (wm8741_i2c_one, wm8741_reg0_addr, wm8741_reg0_default);
	setReg (wm8741_i2c_one, wm8741_reg1_addr, wm8741_reg1_default);
	setReg (wm8741_i2c_one, wm8741_reg2_addr, wm8741_reg2_default);
	setReg (wm8741_i2c_one, wm8741_reg3_addr, wm8741_reg3_default);
	setReg (wm8741_i2c_one, wm8741_reg4_addr, wm8741_reg4_default);
	setReg (wm8741_i2c_one, wm8741_reg5_addr, wm8741_reg5_default);
	setReg (wm8741_i2c_one, wm8741_reg6_addr, wm8741_reg6_default);
	setReg (wm8741_i2c_one, wm8741_reg7_addr, wm8741_reg7_default);
	setReg (wm8741_i2c_one, wm8741_reg8_addr, wm8741_reg8_st_default);

}

///////////////////////////////////////////////
// Set register through I2C bus
///////////////////////////////////////////////
void WM8741::setReg (uint8_t ChipAddress, uint8_t RegAddress, uint8_t RegValue) {
   Wire.beginTransmission(ChipAddress);
   Wire.write(RegAddress);
   Wire.write(RegValue);
   Wire.endTransmission();
}

//////////////////////////////////
// Set volume to indicated level
//////////////////////////////////
void WM8741::SetVolume(int unsigned volume, bool changeMute = true) {
	
	//check flags / revert mute
	if (muted && changeMute) Mute();
	
	//split the 10 bits into 2x 5 bits = 2 regs
	reg0 = (((byte)volume) & 0x1F);
	reg1 = ((byte)(volume>>5));
	reg1action = (reg1 | 0x20); //Set bit6 to 1 for volume setting start on chip - writes reg 0-3
	
	if (isMono) {
		setReg (wm8741_i2c_one, wm8741_reg0_addr, reg0);
		setReg (wm8741_i2c_two, wm8741_reg0_addr, reg0);
		setReg (wm8741_i2c_one, wm8741_reg1_addr, reg1);
		setReg (wm8741_i2c_two, wm8741_reg1_addr, reg1);
		setReg (wm8741_i2c_one, wm8741_reg2_addr, reg0);
		setReg (wm8741_i2c_two, wm8741_reg2_addr, reg0);
		setReg (wm8741_i2c_one, wm8741_reg3_addr, reg1action);
		setReg (wm8741_i2c_two, wm8741_reg3_addr, reg1action);
		
	}
	else {
		setReg (wm8741_i2c_one, wm8741_reg0_addr, reg0);
		setReg (wm8741_i2c_one, wm8741_reg1_addr, reg1);
		setReg (wm8741_i2c_one, wm8741_reg2_addr, reg0);
		setReg (wm8741_i2c_one, wm8741_reg3_addr, reg1action);
	}

}

///////////////////////////////////////////////
// Set FIR filters (1 out of 5)
///////////////////////////////////////////////
void WM8741::SetFilter(int filter) {
	//Effect of filter change according to sample rate
	//switch / case 
	//bit oprtaion
	//setreg
	byte filtre1 = 0x00;
	byte filtre2 = 0x01;
	byte filtre3 = 0x02;
	byte filtre4 = 0x03;
	byte filtre5 = 0x04;
	
	byte reg6 = ((byte) getReg(wm8741_i2c_one, wm8741_reg6_addr));
	
	switch (filter) {
		case 1:
			reg6 = (reg6 | filter1); //default value
			FilterActive = 1;
			break;
		case 2:
			reg6 = (reg6 | filter2);
			FilterActive = 2;
			break;
		case 3:
			reg6 = (reg6 | filter3);
			FilterActive = 3;
			break;
		case 4:
			reg6 = (reg6 | filter4);
			FilterActive = 4;
			break;
		case 5:
			reg6 = (reg6 | filter5);
			FilterActive = 5;
			break;
		default:
			reg6 = (reg6 | filter4);
			FilterActive = 5;
	}

	if (isMono) {
		setReg (wm8741_i2c_one, wm8741_reg6_addr, reg6);
		setReg (wm8741_i2c_two, wm8741_reg6_addr, reg6);		
	}
	else {
		setReg (wm8741_i2c_one, wm8741_reg6_addr, reg6);
	}
	
	
}

/////////////////////////////////
// Set next filter
/////////////////////////////////
void WM8741::NextFilter(void) {
	
	if (FilterActive == 6) FilterActive = 0;
	SetFilter(++FilterActive);

}

/////////////////////////////////
// Set previous filter
/////////////////////////////////
void WM8741::PrevFilter(void) {
	
	if (FilterActive == 0) FilterActive = 6;
	SetFilter(--FilterActive);

}

/////////////////////////////////
// Increase volume by 1 dB
/////////////////////////////////
void WM8741::VolumeUp(void) {
	
	CurrentVolume -= 8; //decrease attenuation by 1dB (8x0.125dB)
	SetVolume(CurrentVolume, true);

}

/////////////////////////////////
// Decrease volume by 1 dB
/////////////////////////////////
void WM8741::VolumeDown(void) {

	CurrentVolume += 8; //increase attenuation by 1dB (8x0.125dB)
	SetVolume(CurrentVolume, true);
}



///////////////////////////
// Mute / Unmute function
// flip flop mode
///////////////////////////
void WM8741::Mute(void) {
	if (muted) {		
		reg4 = wm8741_reg4_default & 0x17; //set bit4 to 0
		if (isMono) {
			setReg (wm8741_i2c_one, wm8741_reg4_addr, reg4);
			setReg (wm8741_i2c_two, wm8741_reg4_addr, reg4);
		}
		else {
			setReg (wm8741_i2c_one, wm8741_reg4_addr, reg4);
		}
		muted = false;
	}
	else {
		reg4 = wm8741_reg4_default | 0x08; //set bit4 to 1
		if (isMono) {
			setReg (wm8741_i2c_one, wm8741_reg4_addr, reg4);
			setReg (wm8741_i2c_two, wm8741_reg4_addr, reg4);
		}
		else {
			setReg (wm8741_i2c_one, wm8741_reg4_addr, reg4);
		}
		muted = true;
	}
}


///////////////////////////
// Set volume to -127.5 dB
///////////////////////////

void WM8741::ZeroSound(void) {
	
	CurrentVolume = wm8741_VolMin;
	SetVolume(CurrentVolume, false);
}


///////////////////////////
// Soft reset of the Dac chip
///////////////////////////
void WM8741::SoftReset(void){
		if (isMono) {
			setReg (wm8741_i2c_one, wm8741_reg9_addr, 0x00);
			setReg (wm8741_i2c_two, wm8741_reg9_addr, 0x00);
		}
		else {
			setReg (wm8741_i2c_one, wm8741_reg9_addr, 0x00);
		}
}

///////////////////////////
// Read one register
///////////////////////////
uint8_t WM8741::getReg(uint8_t ChipAddress, uint8_t RegAddress){
	
	byte reg = 0x00;
	Wire.beginTransmission(ChipAddress);
	Wire.send(RegAddress);
	Wire.endTransmission();
	Wire.requestFrom(ChipAddress,1);
	if (Wire.available()) reg = Wire.receive();
	return reg;
}

/////////////////////////////////
// retrieve the Mute flag in Reg4
/////////////////////////////////

void WM8741::CheckMuteStatus (void) {

	byte status = (((byte) getReg(wm8741_i2c_one, wm8741_reg9_addr)) & 0x08); //assume volume control reg4 is equivalent in mono or stereo mode
	if (status == 0x08) {
		muted = true;
	}
	else {
		muted = false;
	}
}

bool WM8741::isMuted (void) {
	return muted;
}

