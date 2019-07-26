// RedioSwitch.h

#ifndef _REDIOSWITCH_h
#define _REDIOSWITCH_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#include "PinDefine.h"
#include "NexObject.h"
#include "AlertSoundEffect.h"

void avoid_callback(void *ptr);

// 麥克風上面的六顆按鈕
class RadioControlButton {
public:
	void init();
	void read_hand_hold_mic_status(unsigned long now);
	void switch_alert_power(bool status);
	void radio_host_power(bool status);
	void sperker_host_power(bool status);
	bool get_sperker_host_power_status();
private:
	unsigned long time_stamp = 0;
	void display_mic_tutorial(unsigned long now, int cmd_code=-3);
	void switch_alert_sound_mute(int sig);
	int channel_now = 0;
	bool sperker_host_power_status;	// true = on; false = off;
};

// 初始化
void RadioControlButton::init() {
	Serial1.begin(9600);
	
	pinMode(HAND_HELD_MIC_OUT_PIN, OUTPUT);
	pinMode(ALERT_OUT_PIN, OUTPUT);
	pinMode(COCKPIT_OUT_PIN, OUTPUT);

	pinMode(ALERT_POWER, OUTPUT);
	pinMode(ALERT_MIC_POWER_PIN, OUTPUT);
	pinMode(SECEND_RADIO_POWER_PIN, OUTPUT);
	sperker_host_power(false);
}

void RadioControlButton::read_hand_hold_mic_status(unsigned long now) {
	if (now - time_stamp < 100) { return; }
	time_stamp = now;
	int active_channel = 0;
	// 讀取命令
	if (Serial1.available() <= 0) { return; }

	char cmd[10] = { '\n' };
	int index = 0;
	while (Serial1.available())
	{
		char c = Serial1.read();
		if (index < 10) {
			cmd[index++] = c;
		}
		delay(1);
	}

	int cmd_code = strncmp(cmd, "0", 1);
	static int last_cmd_code = -3;
	// 防止雜訊
	if (cmd_code != last_cmd_code) {
		last_cmd_code = cmd_code;
		return;
	}
	if (cmd_code != -3) {
		Serial.println(cmd_code);
	}
	display_mic_tutorial(now, cmd_code);
	switch (cmd_code)
	{
	case -3:
		switch_alert_sound_mute(cmd_code);
		break;
	case 0:
		avoid_callback(&b_aud_left);
		break;
	case 1:
		avoid_callback(&b_aud_front);
		break;
	case 2:
		avoid_callback(&b_aud_right);
		break;
	case 3:
		active_channel = 1;
		break;
	case 4:
		active_channel = 2;
		break;
	case 5:
		active_channel = 3;
		break;
	case 6:
		switch_alert_sound_mute(cmd_code);
		break;
	default:
		// 無意義訊號
		return;
		break;
	}
	//Serial.print("cmd: ");
	//Serial.println(cmd);

	digitalWrite(HAND_HELD_MIC_OUT_PIN, active_channel == 1 ? HIGH:LOW);		// 無線電發話
	radio_host_power(active_channel == 1 ? true : false);
	digitalWrite(COCKPIT_OUT_PIN, active_channel == 2 ? HIGH : LOW);		// 人員座艙
	digitalWrite(ALERT_OUT_PIN, active_channel == 3 ? HIGH : LOW);		// 警報器
	switch_alert_power(active_channel == 3 ? true : false);
	
}
// 警報器通話按下(關閉時會接通警報音&避讓音)
void RadioControlButton::switch_alert_power(bool status) {
	digitalWrite(ALERT_POWER, status ? HIGH:LOW);
}
// 無線電主機電源
void RadioControlButton::radio_host_power(bool status) {
	digitalWrite(ALERT_MIC_POWER_PIN, status ? HIGH : LOW);
}
// 揚聲器主機電源
void RadioControlButton::sperker_host_power(bool status) {
	sperker_host_power_status = status;
	digitalWrite(SECEND_RADIO_POWER_PIN, status ? HIGH : LOW);
}

// 查詢揚聲器主機電源狀態
bool RadioControlButton::get_sperker_host_power_status() {
	return this->sperker_host_power_status;
}

void RadioControlButton::display_mic_tutorial(unsigned long now, int cmd_code = -3) {
	//return;
	static int last_cmd_code = -3;
	NexVariable* var_array[7] = {
		&va_mic_b1,
		&va_mic_b2,
		&va_mic_b3,
		&va_mic_b0,
		&va_mic_b4,
		&va_mic_b5,
		&va_mic_b6,
	};

	if (last_cmd_code == cmd_code) { return; }
	if (last_cmd_code != -3) {
		var_array[last_cmd_code]->setValue(0);
	}

	last_cmd_code = cmd_code;
	if (cmd_code != -3) {
		var_array[cmd_code]->setValue(1);
	}
	

}
void RadioControlButton::switch_alert_sound_mute(int signal) {
	static int last_signal = -3;
	if (signal == last_signal) { return;}
	last_signal = signal;
	if (signal == 6) {
		sirenVolume = sirenVolume == 10 ? 3:10;
	}
}

#endif