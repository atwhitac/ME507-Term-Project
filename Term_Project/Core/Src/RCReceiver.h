/*
 * RCReceiver.h
 *
 *  Created on: May 13, 2024
 *      Author: andyr
 */

#ifndef SRC_RCRECEIVER_H_
#define SRC_RCRECEIVER_H_
#include "stm32f4xx_hal.h"

class RCReceiver {
public:
	TIM_HandleTypeDef* timer; // like &htim3
	uint32_t channel;
	uint16_t count1;
	uint16_t count2;
	uint16_t diff;
	uint8_t First_Capture;
	int16_t signal;
	uint8_t flag;
	uint8_t test;

	  RCReceiver(TIM_HandleTypeDef* timer, uint32_t channel);

	  void trigger_signal();

	  int16_t get_signal();

	  uint8_t check_flag();

	  void set_flag(uint8_t n);
};

#endif /* SRC_RCRECEIVER_H_ */
