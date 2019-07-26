// DelayQueue.h

#ifndef _DELAYQUEUE_h
#define _DELAYQUEUE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "NextionCallback.h"
#define QUEUE_BUFFER_SIZE 10
#define QUEUE_DELAY_TIME 500
struct PtrStore {
	void *ptr;
	unsigned long time_stamp;
};
class DelayQueue {
public:
	DelayQueue() {
		for (int i = 0; i < QUEUE_BUFFER_SIZE; i++) {
			this->queue[i] = { NULL, 0 };
		}
	}
	/**
	* 把指針加入序列等待處理
	* @param ptr 待加入的指針
	* @return 是否加入成功
	*/
	bool append_pointer(void *ptr) {
		for (int i = 1; i < QUEUE_BUFFER_SIZE; i++) {
			int index = this->queue_index + i;
			index = index >= QUEUE_BUFFER_SIZE ? index - QUEUE_BUFFER_SIZE : index;
			if (queue[index].ptr == NULL) {
				queue[index].ptr = ptr;
				queue[index].time_stamp = millis();
				return true;
			}
		}
		return false;
	};
	void (*callback_function)(void *ptr) = NULL;
	void poll() {
		if (callback_function == NULL) { return; }
		if (millis() - this->queue[queue_index].time_stamp < QUEUE_DELAY_TIME) { return; }
		callback_function(this->queue[queue_index].ptr);
		this->next_queue_index();
	}
private:
	PtrStore queue[QUEUE_BUFFER_SIZE] = {};		// 儲存指針的列隊
	int queue_index = 0;		// 當前準備處理的指針
	/**
	* 把當前處理的指針移動到下一個位置
	*/
	void next_queue_index() {
		queue[queue_index].ptr = NULL;
		queue_index++;
		queue_index = queue_index >= QUEUE_BUFFER_SIZE ? 0 : queue_index;
	}
};

#endif

