// PinDefine.h

#ifndef _PINDEFINE_h
#define _PINDEFINE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#endif

// BY8301
#define IO1 31
#define IO2 33
#define IO3 35
#define BUSY 37
#define AUD_SOURCE_0 23
#define AUD_SOURCE_1 25
#define AUD_SOURCE_2 27
#define ALERT 39    // 硬控警報因
#define MCP41010_CS_PIN 53
#define LIGHT_LINE_PIN 40	// 硬控排燈
#define LIGHT_FLASH_PIN 42	// 硬控暴閃


#ifndef HAND_HELD_MIC_OUT_PIN
#define HAND_HELD_MIC_OUT_PIN 28
#endif // !HAND_HELD_MIC_OUT_PIN

#ifndef ALERT_OUT_PIN
#define ALERT_OUT_PIN 30
#endif // !ALERT_OUT_PIN

#ifndef COCKPIT_OUT_PIN
#define COCKPIT_OUT_PIN 32
#endif // !COCKPIT_OUT_PIN

#ifndef ALERT_POWER_PIN
#define ALERT_POWER 44
#endif // !ALERT_POWER

#ifndef ALERT_MIC_POWER_PIN
#define ALERT_MIC_POWER_PIN 48
#endif // !ALERT_MIC_POWER_PIN

#ifndef SECEND_RADIO_POWER_PIN
#define SECEND_RADIO_POWER_PIN 46
#endif // !SECEND_RADIO_POWER_PIN