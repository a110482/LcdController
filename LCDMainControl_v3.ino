#include "FlashAndLineLightController.h"
#include "LibPath.h"
#include "NextionExtension.h"
#include "MCP4101Controller.h"
#include "NextionCallback.h"
#include "BY8301.h"
#include "PinDefine.h"
#include "AlertSoundEffect.h"
#include "NextionObjectsDefine.h"
#include "RadioControlButton.h"
#include "HmiDataManager.h"
#include <MemoryFree.h>

#define VIRTUAL_MODBUS false	   // 是否開啟模擬modbus功能
#define VERSION 2.06
/* TODO:

*/

// ---- Modbus ----
#include <ModbusRtu.h>

uint16_t au16data[DATA_LEN*2] = {0};  // 前半段是send 後半段是receive
uint16_t binary_data[BINARY_DATA_LEN*2] = {0};
uint8_t u8state;
Modbus slave(2, 2, 34);	

// ---- HMI ----

RadioControlButton radio_control_button = RadioControlButton();
MCP4101Controller mcp41010 = MCP4101Controller();
void setup(void) {
    slave.begin(57600);
	//slave.setTimeOut(100);
	//slave.setTxendPinOverTime(200);
    // ---- Modbus ----
	radio_control_button.init();
	mcp41010.setup();
	pinMode(LIGHT_LINE_PIN, OUTPUT);
	pinMode(LIGHT_FLASH_PIN, OUTPUT);

    // ---- HMI ----
    // 一般按鈕物件
    nexInit();
	attach_hmi_touch_callback_func();
	attach_all_hmi_user_func();
	
	// 初始化螢幕數值
	reset_hmi_current_data(NULL);
	
	// ----設置延遲歸零訊列的處理函數----
	set_zero_delay_queue.callback_function = set_zero;
    
    // ---- 音效 ----
    init_BY8301Controller();
    init_audio_pin_mode();

	// ---- 存檔功能 ----
	read_hmi_eeprom();
    
    Serial.begin(9600);
    Serial.print("LCDMainControl  ");
	Serial.print(VERSION);
	Serial.println("  init");
	//p_home.show();
}

void loop(void) {
	unsigned long now = millis();
	// 讀取手持mic按鈕狀態
	radio_control_button.read_hand_hold_mic_status(now);
	// 模擬modnus
    //virtual_modbus_binary();
    // 監聽螢幕按鈕
    nexLoop(nex_listen_list);
    
    // 把二進制資料寫入modbus陣列
    int data_buffer;
    data_buffer = encode_binary(BINARY_DATA_LEN, binary_data);
    au16data[0] = data_buffer;
	
    // 讀取 ModBus
    slave.poll(au16data, DATA_LEN*2);
    //virtual_modbus_all();
    
    // 還原modbus陣列資料 到二進制資料陣列
    data_buffer = au16data[DATA_LEN];
    decode_binary(data_buffer, binary_data);


    // 更新資料到螢幕
	send_data_to_nextion_hmi_loop();
	// 模擬實體按鈕的延遲歸零訊號
	set_zero_delay_queue.poll();
	
    // 音效播放完畢
    bool busy_now = digitalRead(BUSY);
    if (busy_now != busy_status){
        if (busy_now){
            Serial.println("BY8301 busy!!");
			digitalWrite(IO1, HIGH);
			digitalWrite(IO2, HIGH);
			digitalWrite(IO3, HIGH);
        }else{
            Serial.println("BY8301 not busy!!");
			b_aud_left.data_link->send_val_to_hmi = 0;
        }
        busy_status = busy_now;
    }
    // 警報音
    playSirenSound();
    
    // for playBeep1
    unsigned long t_now = micros(); //us
	
    // debug
    if (Serial.available()) {
		char cmd[20] = { '\0' };
		int index = 0;
		while (Serial.available()) {
			cmd[index++] = Serial.read();
			delay(5);
		}
		if (strcmp(cmd, "help") == 0) {
			Serial.println("b_data\t看二進位資料詳細內容");
			Serial.println("data\t看資料詳細內容");
			Serial.println("rom -i\t查看記錄模式在記憶體裡的數值");
			Serial.println("rom -r\t初始化記憶體裡的數值");
			Serial.println("aud\t看音源狀態");
			delay(100);
			Serial.println("relay off\t切斷所有relay");
			Serial.println("relay on\t打開所有relay");
			Serial.println("vol -v [0-9]\t設置音量");
			Serial.println("vol -n\t查看現在設定的音量");
			delay(100);
			Serial.println("light_test\t發送排燈訊號");
			Serial.println("flash_test\t發送暴閃燈訊號");
		}
		if (strcmp(cmd, "b_data") == 0) {
			show_binary_data();
		}
		if (strcmp(cmd, "data") == 0) {
			show_data();
		}
		if (strcmp(cmd, "rom -r") == 0) {
			EEPROMHmiDataSave mode_data = get_save_data();
			show_save_data(mode_data);
		}
		if (strcmp(cmd, "rom -i") == 0) {
			init_save_mode();
		}
		if (strcmp(cmd, "aud") == 0) {
			Serial.print("aud source status: ");
			Serial.print(digitalRead(8));
			Serial.print(" ");
			Serial.print(digitalRead(9));
			Serial.print(" ");
			Serial.println(digitalRead(10));
		}
		if (strcmp(cmd, "relay off") == 0) {
			radio_control_button.switch_alert_power(false);
			radio_control_button.radio_host_power(false);
			radio_control_button.sperker_host_power(false);
			Serial.println("relay_off");
		}
		if (strcmp(cmd, "relay on") == 0) {
			radio_control_button.switch_alert_power(true);
			radio_control_button.radio_host_power(true);
			radio_control_button.sperker_host_power(true);
			Serial.println("relay_on");
		}
		if (strncmp(cmd, "vol -v ", 7) == 0) {
			mcp41010.change_value((cmd[7] - '0') );
			Serial.print("變更音量數值: ");
			Serial.println((cmd[7] - '0'));
		}
		if (strcmp(cmd, "vol -n") == 0) {
			Serial.print("音量數值: ");
			Serial.println(mcp41010.level_now);
		}
		if (strcmp(cmd, "light_test") == 0) {
			Serial.println("light_test"); 
			b_light_h_control(&b_light_h, &va_light_h);
		}
		if (strcmp(cmd, "flash_test") == 0) {
			b_light_flash_control(&b_light_flash, &va_light_flash);
			Serial.println("flash_test");
		}
		if (strcmp(cmd, "tailgate") == 0) {
			au16data[16] = au16data[16] == 32 ? 0:32;
			Serial.println("tailgate");
		}
		if (strcmp(cmd, "test") == 0) {
			au16data[19] = au16data[19] == 1 ? 0 : 1;
			Serial.println("test");
		}
		if (strcmp(cmd, "tank0") == 0) {
			*va_tank_0.data_link->recive_val_from_modbus_ptr = 70;
		}
	}
}
 


// 讀取EEPROM裡的資料
void read_hmi_eeprom(){
    // 紀錄模式
    EEPROMHmiDataSave mode_data = get_save_data();
	if (mode_data.mode_code > 5 || mode_data.mode_code<0) {
		Serial.println("led 存檔可能尚未初始化!!!");
		delay(100);
		Serial.println("led 存檔可能尚未初始化!!!");
		delay(100);
		Serial.println("led 存檔可能尚未初始化!!!");
		delay(100);
		Serial.println("輸入 i 來進行記憶體初始化");
		init_save_mode();
	}
    
	// 寫入modbus
    va_led_mode.data_link->send_val_to_hmi = *va_led_mode.data_link->send_val_to_modbus_ptr = mode_data.mode_code;
	va_led_speed.data_link->send_val_to_hmi = *va_led_speed.data_link->send_val_to_modbus_ptr = mode_data.speed;
	*va_volume.data_link->send_val_to_modbus_ptr = va_volume.data_link->send_val_to_hmi = mode_data.volume;
	*b_sync_light.data_link->send_val_to_modbus_ptr = b_sync_light.data_link->send_val_to_hmi = mode_data.sync_light;
	mcp41010.change_value(mode_data.volume);
	// 寫入螢幕
	char page_name[10] = "opening";
	//va_led_mode.setGlobalValue(page_name, mode_data.mode_code);
	//va_led_speed.setGlobalValue(page_name, mode_data.speed);
	//va_volume.setGlobalValue(page_name, mode_data.volume);
}

/**
 * @param data_len  資料長度
 * @param data  2進位資料陣列
 * @return  10進制數字
 */
int encode_binary(int data_len, const uint16_t data[]){
    int res = 0;
    int int_buff;
    for(int i = 0; i<data_len; i++){
        int_buff = data[i]%2;
        int_buff = int_buff << i;
        res += int_buff;
    }
    return res;
}

/**
 * @param data   輸入10進制數字
 * @param res_len  存放結果的緩衝區大小
 * @param start_index  放置結果的起點
 * @param res   存結果的陣列
 * @return  總結果長度
 */
void decode_binary(const int data, uint16_t res[BINARY_DATA_LEN*2]){
	int data_buffer = data;
    for(int index = BINARY_DATA_LEN; index<BINARY_DATA_LEN*2; index++){
        res[index] = data_buffer %2;
		data_buffer /= 2;
    }
}

// 模擬modbus 功能
void virtual_modbus_all(){
    if(!VIRTUAL_MODBUS){return;}
    for(int i = 0; i<DATA_LEN; i++){
        au16data[DATA_LEN+i] = au16data[i];
    }
}
void virtual_modbus_binary(){
    if(!VIRTUAL_MODBUS){return;}
    for(int i = 0; i<BINARY_DATA_LEN; i++){
        binary_data[i] = binary_data[i+BINARY_DATA_LEN];
    }
}
// 列印按鈕們的狀態
void show_binary_data(){
    Serial.println();
    Serial.println("-----------------------");
	Serial.println("index");
	for (int i = 0; i < BINARY_DATA_LEN; i++) {
		Serial.print(i);
		Serial.print("  ");
	}
	Serial.println();
    Serial.println("send:");
    for(int i=0;i<BINARY_DATA_LEN;i++){
        Serial.print(binary_data[i]);
        Serial.print("  "); 
    }
    Serial.println();
    Serial.println("recive:");
    for(int i=BINARY_DATA_LEN;i<BINARY_DATA_LEN*2;i++){
        Serial.print(binary_data[i]);
        Serial.print("  ");
    }
    Serial.println();
    Serial.println("-----------------------");
}
// 列印modbus 資料
void show_data() {
	Serial.println();
	Serial.println("-----------------------");
	Serial.println("send:");
	for (int i = 0; i < DATA_LEN; i++) {
		Serial.print(au16data[i]);
		Serial.print("  ");
	}
	Serial.println();
	Serial.println("recive:");
	for (int i = DATA_LEN; i < DATA_LEN * 2; i++) {
		Serial.print(au16data[i]);
		Serial.print("  ");
	}
	Serial.println();
	Serial.println("hmi:");
	for (int i = 0; i < DATA_LEN; i++) {
		Serial.print(au16data[i]);
		Serial.print("  ");
	}
	Serial.println();
	Serial.println("-----------------------");
}
