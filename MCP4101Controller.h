// MCP4101Controller.h

#ifndef _MCP4101CONTROLLER_h
#define _MCP4101CONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <SPI.h>
#include "PinDefine.h"

class MCP4101Controller
{
public:
	MCP4101Controller();
	void setup();
	void change_value(int level);
	int level_now = 0; // for debug
private:
	int max_level = 10;
	int level_val[10] = {0, 2, 5, 10, 20, 40, 80, 150, 230, 255};
	void MCP41010Write(byte value);
};

MCP4101Controller::MCP4101Controller(){}
void MCP4101Controller::setup() {
	pinMode(MCP41010_CS_PIN, OUTPUT);
	SPI.begin();
}
// 把輸入的音量轉換成10級
/**
* @param volume 音量 0~10
*/
void MCP4101Controller::change_value(int volume) {
	int level = volume;
	// overflow protection
	if (level < 0) { level = 0; }
	if (level > 9) { level = 9; }
	this->level_now = level;
	this->MCP41010Write((byte) this->level_val[level]);
}

// spi 操作
void MCP4101Controller::MCP41010Write(byte value){
	// Note that the integer vale passed to this subroutine
	// is cast to a byte
	digitalWrite(MCP41010_CS_PIN, LOW);
	SPI.transfer(0x11); // This tells the chip to set the pot
	SPI.transfer(value);     // This tells it the pot position
	digitalWrite(MCP41010_CS_PIN, HIGH);
}







#endif

