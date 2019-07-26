// NextionExtension.h

#ifndef _NEXTIONEXTENSION_h
#define _NEXTIONEXTENSION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "LibPath.h"

class NexDataLink {
public:
	NexDataLink(uint16_t *send_val_to_modbus_ptr, uint16_t *recive_val_from_modbus_ptr, uint16_t send_val_to_hmi = 0) {
		this->send_val_to_modbus_ptr = send_val_to_modbus_ptr;
		this->recive_val_from_modbus_ptr = recive_val_from_modbus_ptr;
		this->send_val_to_hmi = send_val_to_hmi;
	}
	/**
	* @param hmi_func 顯示到螢幕的函數
	* @param display_ptr 給hmi_func函數用的參數
	* @param display_ptr2 給hmi_func函數用的參數
	*/
	void attach_hmi_user_func(void(*hmi_func)(void *ptr, void *ptr2), void *display_ptr=NULL, void *display_ptr2 = NULL) {
		this->hmi_func = hmi_func;
		this->display_ptr = display_ptr;
		this->display_ptr2 = display_ptr2;
	}

	void render_if_need() {
		if (hmi_func == NULL) { return; }
		hmi_func(display_ptr, display_ptr2);
	};
	uint16_t *send_val_to_modbus_ptr = NULL;
	uint16_t *recive_val_from_modbus_ptr = NULL;
	uint16_t send_val_to_hmi = 0;
	uint16_t current_data_of_hmi = 0;
private:
	void(*hmi_func)(void *ptr, void *ptr2) = NULL;
	void *display_ptr = NULL;
	void *display_ptr2 = NULL;
};


/**
	擴充存取全域變數的功能
*/

class NexVariableCostom: public NexVariable
{
public:
	NexDataLink *data_link = NULL;
	NexVariableCostom(uint8_t pid, uint8_t cid, const char *name, NexDataLink *data_link = NULL);
	/**
	 * Get val attribute of component
	 * @param global_name 全域變數儲存的頁面名稱
	 * @param number - buffer storing data retur
	 * @return the length of the data
	 */
	uint32_t getGlobalValue(char global_name[10], uint32_t *number);

	/**
	 * @param global_name 全域變數儲存的頁面名稱
	 * @param number
	 * @return
	 */
	bool setGlobalValue(char global_name[10], uint32_t number);
};

// 沿用 NexVariable 的初始化方法
NexVariableCostom::NexVariableCostom(uint8_t pid, uint8_t cid, const char *name, NexDataLink *data_link = NULL): NexVariable(pid, cid, name){
	this->data_link = data_link;
}

bool NexVariableCostom::setGlobalValue(char global_name[10], uint32_t number)
{
	char buf[10] = { 0 };
	String cmd;
	utoa(number, buf, 10);
	cmd += global_name;
	cmd += ".";
	cmd += getObjName();
	cmd += ".val=";
	cmd += buf;
	sendCommand(cmd.c_str());
	return recvRetCommandFinished();
}

uint32_t NexVariableCostom::getGlobalValue(char global_name[10], uint32_t *number)
{
	String cmd = String("get ");
	cmd += global_name;
	cmd += ".";
	cmd += getObjName();
	cmd += ".val";
	sendCommand(cmd.c_str());
	return recvRetNumber(number);
}


class NexButtonCostom:public NexButton{
public:
	NexDataLink *data_link = NULL;
	NexButtonCostom(uint8_t pid, uint8_t cid, const char *name, NexDataLink *data_link) :NexButton(pid, cid, name) {
		this->data_link = data_link;
	};
};



#endif

