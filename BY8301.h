// BY8301.h

#ifndef _BY8301_h
#define _BY8301_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif



#include "PinDefine.h"
// 音效順序
enum Audio { AVOID_RESET, AVOID_LEFT, AVOID_FRONT, AVOID_RIGHT };
void by8301_play_audio(Audio cmd);
bool busy_status = false;
// 初始化音效設定
void init_BY8301Controller() {
	pinMode(IO1, OUTPUT);
	pinMode(IO2, OUTPUT);
	pinMode(IO3, OUTPUT);
	pinMode(BUSY, INPUT);
	pinMode(ALERT, INPUT_PULLUP);

	digitalWrite(IO1, 1);
	digitalWrite(IO2, 1);
	digitalWrite(IO3, 1);
}

// 播放聲音
void by8301_play_audio(Audio cmd) {
	//Serial.println("by8301_play_audio");
	digitalWrite(IO1, cmd == 1 ? 0 : 1);
	digitalWrite(IO2, cmd == 2 ? 0 : 1);
	digitalWrite(IO3, cmd == 3 ? 0 : 1);
};

// 初始化輸出音源控制狀態
void init_audio_pin_mode() {
	pinMode(AUD_SOURCE_0, OUTPUT);
	pinMode(AUD_SOURCE_1, OUTPUT);
	pinMode(AUD_SOURCE_2, OUTPUT);
}

// 輸出音源狀態
void output_aud_source_status(int code) {
	digitalWrite(AUD_SOURCE_0, (code == 0 ? 1 : 0));
	digitalWrite(AUD_SOURCE_1, (code == 1 ? 1 : 0));
	digitalWrite(AUD_SOURCE_2, (code == 2 ? 1 : 0));
}
#endif