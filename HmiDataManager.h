//
// Created by elijah on 2019/1/23.
//

#ifndef LIBRARIES_HMIDATAMANAGER_H
#define LIBRARIES_HMIDATAMANAGER_H

#include <EEPROM.h>


struct EEPROMHmiDataSave{
    int mode_code;    // led 模式代碼
    int bright;     // 亮度
    int speed;      // 速度
	int volume;	// 音量
	bool sync_light;	// 連動排燈暴閃
};

const EEPROMHmiDataSave default_mode = {0, 100, 100, 9, false};

/**
 * @param need  是否需要重設記憶體
 */
void init_save_mode(){
    EEPROMHmiDataSave new_obj = default_mode;
    EEPROM.put(0, new_obj);
    Serial.println("led 存檔初始化完成");
}
/**
 * @param index index of data usually equal to  EEPROMHmiDataSave.mode_id
 * @return EEPROMHmiDataSave object
 */
EEPROMHmiDataSave get_save_data(){
    EEPROMHmiDataSave res;
    EEPROM.get(0, res);
    return res;
}

/**
 * @param new_data  新的要存的資料
 * @param index  要存的位置
 */
void put_save_data(EEPROMHmiDataSave new_data){
    EEPROM.put(0, new_data);
}

/**
 *  列印出來看
 */
void show_save_data(EEPROMHmiDataSave data){
    Serial.println("--------------------");
    Serial.print("mode_code: ");
    Serial.println(data.mode_code);
    Serial.print("bright: ");
    Serial.println(data.bright);
    Serial.print("speed: ");
    Serial.println(data.speed);
	Serial.print("volume: ");
	Serial.println(data.volume);
    Serial.println("--------------------");
}
#endif //LIBRARIES_HMIDATAMANAGER_H
