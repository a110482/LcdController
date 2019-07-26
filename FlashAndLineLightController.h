// FlashAndLineLightController.h

#ifndef _FLASHANDLINELIGHTCONTROLLER_h
#define _FLASHANDLINELIGHTCONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "NextionExtension.h"
#include "HmiDataManager.h"

class FlashAndLineLightController {
public:
	FlashAndLineLightController(NexButtonCostom *ptr_main, NexButtonCostom *ptr_a, NexButtonCostom *ptr_b) {
		this->ptr_main = ptr_main;
		this->ptr_a = ptr_a;
		this->ptr_b = ptr_b;
	}
	void sync_ptrs_status(NexButtonCostom *leading_ptr) {
		if (!this->sync_status) { return; }
		NexButtonCostom *follow_ptr = leading_ptr == this->ptr_a ? this->ptr_b : this->ptr_a;
		follow_ptr->data_link->send_val_to_hmi = *follow_ptr->data_link->send_val_to_modbus_ptr = leading_ptr->data_link->send_val_to_hmi;
	}
	/**
	* 設定是否要連動
	* @param status	true 代表連動
	*/
	void set_sync_status(bool status) {
		this->sync_status = status;
		EEPROMHmiDataSave data = get_save_data();
		data.sync_light = status;
		put_save_data(data);
	}
private:
	bool sync_status = false;
	NexButtonCostom *ptr_main = NULL;
	NexButtonCostom *ptr_a = NULL;
	NexButtonCostom *ptr_b = NULL;
};

#endif

