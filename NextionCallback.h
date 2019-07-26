// NextionCallback.h

#ifndef _NEXTIONCALLBACK_h
#define _NEXTIONCALLBACK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif



#include "NextionObjectsDefine.h"
#include "BY8301.h"
#include "HmiDataManager.h"
#include "RadioControlButton.h"
#include "FlashAndLineLightController.h"
#include "DelayQueue.h"

void send_data_to_nextion_hmi_loop();
// 在.ino裡
extern uint16_t hmi_data[];
extern uint16_t hmi_binary_data[];
extern uint16_t au16data[];
extern uint16_t binary_data[];
extern RadioControlButton radio_control_button;
extern MCP4101Controller mcp41010;
FlashAndLineLightController flash_and_line_light_controller = FlashAndLineLightController(&b_sync_light, &b_light_flash, &b_light_h);
/**
* 把迷你實體按鈕的方波訊號放大(維持在1的時間延長)
*/
DelayQueue set_zero_delay_queue = DelayQueue();
// nextion 監聽用
NexTouch *nex_listen_list[] =
{
	// 一般按鈕物件
	&b_light_h,
	&b_light_side,
	&b_light_flash,
	&b_light_inside,
	&b_aud_left,
	&b_aud_front,
	&b_aud_right,
	&b_aud_switch,
	// pages
	&p_home,
	&p_opening,
	// 多數值按鈕物件
	// --聲音
	//&b_aud_source,
	&b_vol_up,
	&b_vol_down,
	&b_speed_dwn,
	&b_speed_up,
	&b_mode_1,
	&b_mode_2,
	&b_mode_3,
	&b_mode_4,
	&b_sync_light,
	&b84_shield,
	&b24_square_on,
	&b25_square_off,
	&b2_audsource_0,
	&b3_audsource_1,
	&b4_audsource_2,
	NULL
};

// ------------螢幕到主板的函數----------------


// 更改modbus預備送出的值(au16data 前半段) 切換 1 & 0
void on_off_type_btn(void *ptr){
	NexButtonCostom *btn_ptr = (NexButtonCostom *)ptr;
	btn_ptr->data_link->send_val_to_hmi = *btn_ptr->data_link->send_val_to_modbus_ptr = btn_ptr->data_link->current_data_of_hmi == 0 ? 1 : 0;
}

// 循環數值讀取數值回呼
void read_value_callback(void *ptr) {
	NexVariableCostom *va_ptr = (NexVariableCostom *)ptr;
	uint32_t new_data;
	char page_name[10] = "opening";
	int data_len = va_ptr->getGlobalValue(page_name, &new_data);
	if (data_len == 0) {
		Serial.println("get value fail");
		read_value_callback(ptr);
		return;
	}
	va_ptr->data_link->send_val_to_hmi = *va_ptr->data_link->send_val_to_modbus_ptr = va_ptr->data_link->current_data_of_hmi = new_data;
	Serial.print("new data: ");
	Serial.println(new_data);
}

/*
* 聲音源
* @param status 切換到哪個頻道 0~2
*/
void aud_callback(void *ptr, uint16_t status) {
	NexVariableCostom *va_ptr = (NexVariableCostom *)ptr;
	// 放上新資料
	*va_ptr->data_link->send_val_to_modbus_ptr = va_ptr->data_link->send_val_to_hmi = status;
	//Serial.println(new_data);
	output_aud_source_status(status);
}
void aud_callback_0(void *ptr) {
	aud_callback(ptr, 0);
}
void aud_callback_1(void *ptr) {
	aud_callback(ptr, 1);
}
void aud_callback_2(void *ptr) {
	aud_callback(ptr, 2);
}

// 設定模式回呼
void led_speed_callback(void *ptr, bool is_up) {
	NexVariableCostom *va_ptr = (NexVariableCostom *)ptr;
	int speed = va_ptr->data_link->current_data_of_hmi;
	speed += is_up ? 10 : -10;
	speed = speed < 0 ? 0 : speed>100 ? 100 : speed;

	va_ptr->data_link->send_val_to_hmi = *va_ptr->data_link->send_val_to_modbus_ptr = speed;
	// 紀錄模式
	EEPROMHmiDataSave mode_data = get_save_data();
	mode_data.speed = speed;
	put_save_data(mode_data);
}
void led_speed_up_callback(void *ptr) {
	led_speed_callback(ptr, true);
}
void led_speed_down_callback(void *ptr) {
	led_speed_callback(ptr, false);
}

void led_mode_callback(void *ptr){
	NexButtonCostom *led_mode_btn_ptr_array[4] = {
		&b_mode_1,
		&b_mode_2,
		&b_mode_3,
		&b_mode_4,
	};
	int mode = 0;
	for (int i = 0; i < 4; i++) {
		if (led_mode_btn_ptr_array[i] == ptr) {
			mode = i;
			break;
		}
	}
	NexButtonCostom *btn_ptr = (NexButtonCostom*)ptr;
	btn_ptr->data_link->send_val_to_hmi = *btn_ptr->data_link->send_val_to_modbus_ptr = mode;
	// 紀錄模式
	EEPROMHmiDataSave mode_data = get_save_data();
	mode_data.mode_code = mode;
	put_save_data(mode_data);
}

// 避讓回呼
void avoid_callback(void *ptr) {
	// 如果電源沒開，不顯示避讓圖示
	if (radio_control_button.get_sperker_host_power_status() == false) {
		return;
	}

	NexButtonCostom *btn_ptr = (NexButtonCostom *)ptr;
	int avoid_code = 0;
	if (ptr == &b_aud_left) {
		avoid_code = 1;
	}
	else if(ptr == &b_aud_front)
	{
		avoid_code = 2;
	}
	else if (ptr == &b_aud_right) {
		avoid_code = 3;
	}
	// 寫入數值
	btn_ptr->data_link->send_val_to_hmi = *btn_ptr->data_link->send_val_to_modbus_ptr = avoid_code;
}

// 揚聲器主機電源
void speaker_host_power_callback(void *ptr) {
	on_off_type_btn(ptr);
	NexButtonCostom *btn_ptr = (NexButtonCostom *)ptr;
	radio_control_button.sperker_host_power(btn_ptr->data_link->send_val_to_hmi == 1 ? true:false);
}

// 音量回呼
void vol_callback(void *ptr, bool is_up) {
	NexVariableCostom *va_ptr = (NexVariableCostom *)ptr;
	int vol_value = va_ptr->data_link->current_data_of_hmi;
	
	vol_value += is_up ? 1 : -1;
	vol_value = vol_value > 9 ? 9 : vol_value < 0 ? 0 : vol_value;
	va_ptr->data_link->send_val_to_hmi = vol_value;

	// 存入記憶體
	EEPROMHmiDataSave mode_data = get_save_data();
	mode_data.volume = vol_value;
	put_save_data(mode_data);
	// 變更電阻值
	mcp41010.change_value(vol_value);
}
void vol_callback_up(void *ptr) {
	vol_callback(ptr, true);
}
void vol_callback_down(void *ptr) {
	vol_callback(ptr, false);
}
// 螢幕直接控制硬控排燈
void light_control_callback(void *ptr) {
	on_off_type_btn(ptr);
	flash_and_line_light_controller.sync_ptrs_status((NexButtonCostom *)ptr);
}
void physical_simulation_push(void *ptr) {
	NexButtonCostom *btn_ptr = (NexButtonCostom *)ptr;
	btn_ptr->data_link->send_val_to_hmi = btn_ptr->data_link->current_data_of_hmi == 0 ? 1 : 0;
	*btn_ptr->data_link->send_val_to_modbus_ptr = 1;
}
void physical_simulation_pop(void *ptr) {
	set_zero_delay_queue.append_pointer(ptr);
}
void set_zero(void *ptr) {
	NexButtonCostom *btn_ptr = (NexButtonCostom *)ptr;
	*btn_ptr->data_link->send_val_to_modbus_ptr = 0;
}
void reset_hmi_current_data(void *ptr);		//  實作在下面
/*
* 方形燈開關
* @param status 方形燈狀態 true 為開啟
*/
void led_square_switch(void *ptr, bool status) {
	NexButtonCostom *btn_ptr = (NexButtonCostom *)ptr;
	*btn_ptr->data_link->send_val_to_modbus_ptr = 
		btn_ptr->data_link->send_val_to_hmi = status ? 1:0;
}
void led_square_switch_on(void *ptr) {
	led_square_switch(ptr, true);
}
void led_square_switch_off(void *ptr) {
	led_square_switch(ptr, false);
}


// 加載按鈕回呼函數
void attach_hmi_touch_callback_func() {
	b_light_h.attachPush(light_control_callback, &b_light_h);
	b_light_side.attachPush(physical_simulation_push, &b_light_side);
	b_light_side.attachPop(physical_simulation_pop, &b_light_side);
	b_light_flash.attachPush(light_control_callback, &b_light_flash);
	b_light_inside.attachPush(physical_simulation_push, &b_light_inside);
	b_light_inside.attachPop(physical_simulation_pop, &b_light_inside);
	b_aud_left.attachPush(avoid_callback, &b_aud_left);
	b_aud_front.attachPush(avoid_callback, &b_aud_front);
	b_aud_right.attachPush(avoid_callback, &b_aud_right);
	b_aud_switch.attachPush(speaker_host_power_callback, &b_aud_switch);
	// 多數值按鈕物件
	// --聲音
	//b_aud_source.attachPush(aud_callback, &va_aud_source);
	b2_audsource_0.attachPush(aud_callback_0, &b2_audsource_0);
	b3_audsource_1.attachPush(aud_callback_1, &b3_audsource_1);
	b4_audsource_2.attachPush(aud_callback_2, &b4_audsource_2);
	b_vol_up.attachPush(vol_callback_up, &va_volume);
	b_vol_down.attachPush(vol_callback_down, &va_volume);
	b_speed_dwn.attachPush(led_speed_down_callback, &va_led_speed);
	b_speed_up.attachPush(led_speed_up_callback, &va_led_speed);
	b_mode_1.attachPush(led_mode_callback, &b_mode_1);
	b_mode_2.attachPush(led_mode_callback, &b_mode_2);
	b_mode_3.attachPush(led_mode_callback, &b_mode_3);
	b_mode_4.attachPush(led_mode_callback, &b_mode_4);

	b_sync_light.attachPush(on_off_type_btn, &b_sync_light);
	b84_shield.attachPush(physical_simulation_push, &b84_shield);
	b84_shield.attachPop(physical_simulation_pop, &b84_shield);
	p_opening.attachPush(reset_hmi_current_data, NULL);
	b24_square_on.attachPush(led_square_switch_on, &b24_square_on);
	b25_square_off.attachPush(led_square_switch_off, &b25_square_off);
}

// ------------主板更新到螢幕上的函數----------------
// nextion 監聽用
NexButtonCostom *nex_display_list[] = {
	&b_pto,
	&b_pto2,
	&b_rolldoor,
	&b_head_lock,
	&b_light_h,
	&b_light_side,
	&b_light_flash,
	&b_light_inside,
	&b_aud_switch,
	&b_aud_left,
	&b_sync_light,
	&b84_shield,
	&b_85_passenger,
	&b86_tailgate,
	&b24_square_on,
	NULL
};
NexVariableCostom *nex_display_list_2[] = {
	&va_battery,
	&va_volume,
	&va_aud_source,
	&va_led_speed,
	&va_led_mode,
	&va_tank_0,
	&va_tank_1,
	&va_tank_2,
	NULL
};

void control_by_modbus(void *btn, void *var) {
	NexVariableCostom *var_ptr = (NexVariableCostom *)var;
	NexButtonCostom *btn_ptr = (NexButtonCostom *)btn;
	if (*btn_ptr->data_link->recive_val_from_modbus_ptr == btn_ptr->data_link->current_data_of_hmi) { return; }
	btn_ptr->data_link->current_data_of_hmi = *btn_ptr->data_link->recive_val_from_modbus_ptr;
	
	//btn_ptr->Set_background_crop_picc(btn_ptr->data_link->current_data_of_hmi);
	//btn_ptr->Set_press_background_crop_picc2(btn_ptr->data_link->current_data_of_hmi);
	char page_name[10] = "opening";
	var_ptr->setGlobalValue(page_name, *btn_ptr->data_link->recive_val_from_modbus_ptr);
};
void control_by_self(void *btn, void *var) {
	NexVariableCostom *var_ptr = (NexVariableCostom *)var;
	NexButtonCostom *btn_ptr = (NexButtonCostom *)btn;
	if (btn_ptr->data_link->send_val_to_hmi == btn_ptr->data_link->current_data_of_hmi) { return; }
	btn_ptr->data_link->current_data_of_hmi = btn_ptr->data_link->send_val_to_hmi;
	
	//btn_ptr->Set_background_crop_picc(btn_ptr->data_link->current_data_of_hmi);
	//btn_ptr->Set_press_background_crop_picc2(btn_ptr->data_link->current_data_of_hmi);
	char page_name[10] = "opening";
	var_ptr->setGlobalValue(page_name, btn_ptr->data_link->send_val_to_hmi);
};
void head_lock_display(void *btn, void *var) {
	static int lock_status = 0;
	NexVariableCostom *var_ptr = (NexVariableCostom *)var;
	NexButtonCostom *btn_ptr = (NexButtonCostom *)btn;
	if (*btn_ptr->data_link->recive_val_from_modbus_ptr == btn_ptr->data_link->current_data_of_hmi) { return; }

	Serial.print("recive_val_from_modbus_ptr:");
	Serial.println(*btn_ptr->data_link->recive_val_from_modbus_ptr);
	Serial.print("current_data_of_hmi:");
	Serial.println(btn_ptr->data_link->current_data_of_hmi);

	btn_ptr->data_link->current_data_of_hmi = *btn_ptr->data_link->recive_val_from_modbus_ptr;
	char page_name[10] = "opening";
	var_ptr->setGlobalValue(page_name, *btn_ptr->data_link->recive_val_from_modbus_ptr);

	if (btn_ptr->data_link->current_data_of_hmi == 0) {
		lock_status = 0;
		p_home.show();
	}
	else if (lock_status == 0) {
		lock_status = 1;
		Serial.println("p_alert  p_alert");
		p_alert.show();
	}

}
void avoid_display(void *btn, void *var) {
	control_by_self(btn, var);
	NexButtonCostom *btn_ptr = (NexButtonCostom *)btn;
	// 播放音效
	Audio cmd = (Audio)btn_ptr->data_link->current_data_of_hmi;
	by8301_play_audio(cmd);
}
void set_global_var_by_modbus(void *btn, void *var) {
	NexVariableCostom *var_ptr = (NexVariableCostom *)var;
	if (*var_ptr->data_link->recive_val_from_modbus_ptr == var_ptr->data_link->current_data_of_hmi) { return; }
	char page_name[10] = "opening";
	var_ptr->setGlobalValue(page_name, *var_ptr->data_link->recive_val_from_modbus_ptr);
	var_ptr->data_link->current_data_of_hmi = *var_ptr->data_link->recive_val_from_modbus_ptr;
	//Serial.println(*var_ptr->data_link->recive_val_from_modbus_ptr);
}
void set_global_var_by_self(void *btn, void *var) {
	NexVariableCostom *var_ptr = (NexVariableCostom *)var;
	if (var_ptr->data_link->send_val_to_hmi == var_ptr->data_link->current_data_of_hmi) { return; }
	char page_name[10] = "opening";
	var_ptr->setGlobalValue(page_name, var_ptr->data_link->send_val_to_hmi);
	var_ptr->data_link->current_data_of_hmi = var_ptr->data_link->send_val_to_hmi;
}
void b_light_h_control(void *btn, void *var) {
	NexButtonCostom *btn_ptr = (NexButtonCostom *)btn;
	if (btn_ptr->data_link->send_val_to_hmi == btn_ptr->data_link->current_data_of_hmi) { return; }
	control_by_self(btn, var);
	digitalWrite(LIGHT_LINE_PIN, btn_ptr->data_link->send_val_to_hmi == 0 ? LOW : HIGH);
}
void b_light_flash_control(void *btn, void *var) {
	NexButtonCostom *btn_ptr = (NexButtonCostom *)btn;
	if (btn_ptr->data_link->send_val_to_hmi == btn_ptr->data_link->current_data_of_hmi) { return; }
	control_by_self(btn, var);
	digitalWrite(LIGHT_FLASH_PIN, btn_ptr->data_link->send_val_to_hmi == 0 ? LOW : HIGH);
}
void sync_light(void *btn, void *var) {
	NexButtonCostom *btn_ptr = (NexButtonCostom *)btn;
	if (btn_ptr->data_link->send_val_to_hmi == btn_ptr->data_link->current_data_of_hmi) { return; }
	control_by_self(btn, var);
	flash_and_line_light_controller.set_sync_status(btn_ptr->data_link->send_val_to_hmi == 1 ? true:false);
}
void reset_hmi_current_data(void *ptr) {
	int  index = 0;
	while (nex_display_list[index] != NULL)
	{
		NexButtonCostom *target = nex_display_list[index++];
		target->data_link->current_data_of_hmi = 0;
	}
	index = 0;
	while (nex_display_list_2[index] != NULL)
	{
		NexVariableCostom *target = nex_display_list_2[index++];
		target->data_link->current_data_of_hmi = 0;
	}

	va_aud_source.data_link->current_data_of_hmi = 2;
	aud_callback(&va_aud_source, 0); // 打開預設音源頻道
}

/*
* 測照之前需要保護自己的狀態，在一秒內不受modbus控制
* 現在回歸由modbus控制
void b_light_side_callback(void *btn, void *var) {
	static unsigned long time_stamp = 0;
	NexVariableCostom *var_ptr = (NexVariableCostom *)var;
	NexButtonCostom *btn_ptr = (NexButtonCostom *)btn;
	uint16_t rev = *btn_ptr->data_link->recive_val_from_modbus_ptr;
	uint16_t set = btn_ptr->data_link->send_val_to_hmi;
	uint16_t current = btn_ptr->data_link->current_data_of_hmi;
	if (rev == set == current){ return; }
}
*/
void attach_all_hmi_user_func() {
	b_pto.data_link->attach_hmi_user_func(control_by_modbus, &b_pto, &va_pto);
	b_pto2.data_link->attach_hmi_user_func(control_by_modbus, &b_pto2, &va_pto2);
	b_rolldoor.data_link->attach_hmi_user_func(control_by_modbus, &b_rolldoor, &va_rolldoor);
	b_head_lock.data_link->attach_hmi_user_func(head_lock_display, &b_head_lock, &va_head_lock);
	b_light_side.data_link->attach_hmi_user_func(control_by_modbus, &b_light_side, &va_light_side);
	b_light_h.data_link->attach_hmi_user_func(b_light_h_control, &b_light_h, &va_light_h);
	b_light_flash.data_link->attach_hmi_user_func(b_light_flash_control, &b_light_flash, &va_light_flash);
	b_light_inside.data_link->attach_hmi_user_func(control_by_modbus, &b_light_inside, &va_light_in);
	b_aud_switch.data_link->attach_hmi_user_func(control_by_self, &b_aud_switch, &va_aud_switch);
	b_aud_left.data_link->attach_hmi_user_func(avoid_display, &b_aud_left, &va_aud_out);
	va_battery.data_link->attach_hmi_user_func(set_global_var_by_modbus, NULL, &va_battery);
	va_tank_0.data_link->attach_hmi_user_func(set_global_var_by_modbus, NULL, &va_tank_0);
	va_tank_1.data_link->attach_hmi_user_func(set_global_var_by_modbus, NULL, &va_tank_1);
	va_tank_2.data_link->attach_hmi_user_func(set_global_var_by_modbus, NULL, &va_tank_2);
	b_sync_light.data_link->attach_hmi_user_func(sync_light, &b_sync_light, &va_sync_light);
	va_volume.data_link->attach_hmi_user_func(set_global_var_by_self, NULL, &va_volume);
	//va_aud_source.data_link->attach_hmi_user_func(set_global_var_by_self, NULL, &va_aud_source);
	b2_audsource_0.data_link->attach_hmi_user_func(control_by_self, &b2_audsource_0, &va_aud_source);
	b84_shield.data_link->attach_hmi_user_func(control_by_modbus, &b84_shield, &va_shield);
	b_85_passenger.data_link->attach_hmi_user_func(control_by_modbus, &b_85_passenger, &va_passenger);
	va_led_speed.data_link->attach_hmi_user_func(set_global_var_by_self, NULL, &va_led_speed);
	va_led_mode.data_link->attach_hmi_user_func(set_global_var_by_self, NULL, &va_led_mode);
	b86_tailgate.data_link->attach_hmi_user_func(control_by_modbus, &b86_tailgate, &va_tailgate);
	b24_square_on.data_link->attach_hmi_user_func(control_by_self, &b24_square_on, &va_squareflash);
}

void send_data_to_nextion_hmi_loop() {
	/*
	static unsigned long time_stamp = 0;
	if (millis() - time_stamp < 100) { return; }
	time_stamp = millis();
	*/
	int  index = 0;
	while (nex_display_list[index] != NULL)
	{
		NexButtonCostom *target = nex_display_list[index++];
		target->data_link->render_if_need();
	}
	index = 0;
	while (nex_display_list_2[index] != NULL)
	{
		NexVariableCostom *target = nex_display_list_2[index++];
		target->data_link->render_if_need();
	}
}

#endif