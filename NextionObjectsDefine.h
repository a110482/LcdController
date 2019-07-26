// NextionObjectsDefine.h

#ifndef _NEXTIONOBJECTSDEFINE_h
#define _NEXTIONOBJECTSDEFINE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Nextion.h"
#define BINARY_DATA_LEN 15
#define DATA_LEN 16 // mod bus 資料長度
// 在.ino裡
extern uint16_t au16data[];
extern uint16_t binary_data[];
// 變數 依照modbus資料陣列順序排序
NexVariableCostom va_battery = NexVariableCostom(0, 2, "va_battery",
	new NexDataLink(&au16data[2], &au16data[2 + DATA_LEN]));
NexVariableCostom va_head_lock = NexVariableCostom(0, 10, "va_head_lock");   // 車頭的訊號
NexVariableCostom va_tank_0 = NexVariableCostom(0, 3, "va_tank_0",
	new NexDataLink(&au16data[4], &au16data[4 + DATA_LEN]));
NexVariableCostom va_tank_1 = NexVariableCostom(0, 4, "va_tank_1", 
	new NexDataLink(&au16data[5], &au16data[5 + DATA_LEN]));
NexVariableCostom va_tank_2 = NexVariableCostom(0, 5, "va_tank_2",
	new NexDataLink(&au16data[6], &au16data[6 + DATA_LEN]));

// 多數值按鈕物件
// --音源
//NexButtonCostom b_aud_source = NexButtonCostom(1, 30, "cb30_audsource",
	//new NexDataLink(&au16data[7], &au16data[7 + DATA_LEN]));
NexButtonCostom b2_audsource_0 = NexButtonCostom(7, 2, "b2_audsource_1",
	new NexDataLink(&au16data[7], &au16data[7 + DATA_LEN]));
NexButtonCostom b3_audsource_1 = NexButtonCostom(7, 3, "b3_audsource_2",
	b2_audsource_0.data_link);
NexButtonCostom b4_audsource_2 = NexButtonCostom(7, 4, "b4_audsource_3",
	b2_audsource_0.data_link);

NexVariableCostom va_aud_source = NexVariableCostom(0, 16, "va_aud_source",
	b2_audsource_0.data_link);
// --音量
NexButtonCostom b_vol_up = NexButtonCostom(1, 81, "b81_vol_up",
	new NexDataLink(&au16data[8], &au16data[8 + DATA_LEN]));
NexButtonCostom b_vol_down = NexButtonCostom(1, 80, "b80_vol_dowm",
	b_vol_up.data_link);
NexVariableCostom va_volume = NexVariableCostom(0, 24, "va_volume",
	b_vol_up.data_link);

NexButtonCostom b_mode_1 = NexButtonCostom(5, 14, "b_mode_1",
	new NexDataLink(&au16data[9], &au16data[9 + DATA_LEN]));
NexButtonCostom b_mode_2 = NexButtonCostom(5, 22, "b_mode_2",
	b_mode_1.data_link);
NexButtonCostom b_mode_3 = NexButtonCostom(5, 21, "b_mode_3",
	b_mode_1.data_link);
NexButtonCostom b_mode_4 = NexButtonCostom(5, 20, "b_mode_4",
	b_mode_1.data_link);
NexVariableCostom va_led_mode = NexVariableCostom(0, 20, "va_led_mode",   // led 模式
	b_mode_1.data_link);

NexButtonCostom b_speed_up = NexButtonCostom(5, 16, "b_speed_up",
	new NexDataLink(&au16data[10], &au16data[10 + DATA_LEN]));
NexButtonCostom b_speed_dwn = NexButtonCostom(5, 15, "b_speed_dwn",
	b_speed_up.data_link);
NexVariableCostom va_led_speed = NexVariableCostom(0, 21, "va_led_speed",   // led 閃爍速度
	b_speed_up.data_link);

NexButtonCostom b_aud_left = NexButtonCostom(1, 11, "b11_speaker_l",
	new NexDataLink(&au16data[11], &au16data[11 + DATA_LEN]));
NexButtonCostom b_aud_front = NexButtonCostom(1, 12, "b12_speaker_f",
	b_aud_left.data_link);
NexButtonCostom b_aud_right = NexButtonCostom(1, 13, "b13_speaker_r",
	b_aud_left.data_link);


// 一般按鈕物件 依照modbus二進位資料陣列排序
NexButtonCostom b_home = NexButtonCostom(1, 1, "b01_home",
	new NexDataLink(&binary_data[0], &binary_data[0 + BINARY_DATA_LEN]));
NexButtonCostom b_pto = NexButtonCostom(1, 2, "b02_pto",
	new NexDataLink(&binary_data[1], &binary_data[1 + BINARY_DATA_LEN]));
NexButtonCostom b_pto2 = NexButtonCostom(1, 3, "b03_pto2",
	new NexDataLink(&binary_data[2], &binary_data[2 + BINARY_DATA_LEN]));
NexButtonCostom b_rolldoor = NexButtonCostom(1, 4, "b04_rolldoor",
	new NexDataLink(&binary_data[3], &binary_data[3 + BINARY_DATA_LEN]));
NexButtonCostom b_light_h = NexButtonCostom(1, 6, "b06_light_h",
	new NexDataLink(NULL, NULL));
NexButtonCostom b_head_lock = NexButtonCostom(1, 5, "b05_headlock",
	new NexDataLink(&binary_data[4], &binary_data[4 + BINARY_DATA_LEN]));
NexButtonCostom b86_tailgate = NexButtonCostom(1, 86, "b86_tailgate",
	new NexDataLink(&binary_data[5], &binary_data[5 + BINARY_DATA_LEN]));
NexButtonCostom b_light_side = NexButtonCostom(1, 7, "b07_light_side",
	new NexDataLink(&binary_data[6], &binary_data[6 + BINARY_DATA_LEN]));
NexButtonCostom b_light_flash = NexButtonCostom(1, 8, "b08_flashlight",
	new NexDataLink(NULL, NULL));
NexButtonCostom b24_square_on = NexButtonCostom(5, 24, "b24_square_on",
	new NexDataLink(&binary_data[7], &binary_data[7 + BINARY_DATA_LEN]));
NexButtonCostom b25_square_off = NexButtonCostom(5, 25, "b25_square_off",
	b24_square_on.data_link);
NexButtonCostom b_light_inside = NexButtonCostom(1, 9, "b09_light_in",
	new NexDataLink(&binary_data[8], &binary_data[8 + BINARY_DATA_LEN]));
NexButtonCostom b_aud_switch = NexButtonCostom(1, 10, "b10_aud_switch",
	new NexDataLink(&binary_data[9], &binary_data[9 + BINARY_DATA_LEN]));
NexButtonCostom b_sync_light = NexButtonCostom(1, 19, "b19_sync_light",
	new NexDataLink(&binary_data[10], &binary_data[10 + BINARY_DATA_LEN]));
NexButtonCostom b84_shield = NexButtonCostom(1, 84, "b84_shield",
	new NexDataLink(&binary_data[11], &binary_data[11 + BINARY_DATA_LEN]));
NexButtonCostom b_85_passenger = NexButtonCostom(1, 85, "b85_passenger",		// 人員座艙指示燈
	new NexDataLink(&binary_data[12], &binary_data[12 + BINARY_DATA_LEN]));


// 不在modbus資料陣列的變數
NexVariableCostom va_pto = NexVariableCostom(0, 7, "va_pto");
NexVariableCostom va_pto2 = NexVariableCostom(0, 8, "va_pto2");
NexVariableCostom va_rolldoor = NexVariableCostom(0, 9, "va_rolldoor");
NexVariableCostom va_light_h = NexVariableCostom(0, 11, "va_light_h");
NexVariableCostom va_light_side = NexVariableCostom(0, 12, "va_light_side");
NexVariableCostom va_light_flash = NexVariableCostom(0, 13, "va_light_flash");
NexVariableCostom va_light_in = NexVariableCostom(0, 14, "va_light_in");
NexVariableCostom va_light_set = NexVariableCostom(0, 15, "va_light_set");
NexVariableCostom va_aud_out = NexVariableCostom(0, 22, "va_aud_out");
NexVariableCostom va_aud_switch = NexVariableCostom(0, 23, "va_aud_switch");
NexVariableCostom va_sync_light = NexVariableCostom(0, 25, "va_sync_light");
NexVariableCostom va_shield = NexVariableCostom(0, 26, "va_shield");
NexVariableCostom va_passenger = NexVariableCostom(0, 27, "va_passenger");
NexVariableCostom va_tailgate = NexVariableCostom(0, 28, "va_tailgate");
NexVariableCostom va_squareflash = NexVariableCostom(0, 29, "va_squareflash", b24_square_on.data_link);

// mic說明頁面
NexVariable va_mic_b0 = NexVariable(6, 11, "va_mic_b0");
NexVariable va_mic_b1 = NexVariable(6, 12, "va_mic_b1");
NexVariable va_mic_b2 = NexVariable(6, 13, "va_mic_b2");
NexVariable va_mic_b3 = NexVariable(6, 14, "va_mic_b3");
NexVariable va_mic_b4 = NexVariable(6, 15, "va_mic_b4");
NexVariable va_mic_b5 = NexVariable(6, 16, "va_mic_b5");
NexVariable va_mic_b6 = NexVariable(6, 17, "va_mic_b6");



// 頁面
NexPage p_opening = NexPage(0, 0, "opening");
NexPage p_home = NexPage(1, 0, "home");
NexPage p_alert = NexPage(2, 0, "alert");



#endif