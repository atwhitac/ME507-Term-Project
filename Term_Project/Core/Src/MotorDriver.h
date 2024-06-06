



#ifndef SRC_MOTORDRIVER_H_
#define SRC_MOTORDRIVER_H_
#include "stm32f4xx_hal.h"

class MotorDriver {
public:
	TIM_HandleTypeDef* timer;
	int32_t  duty;
	uint32_t CH;


	MotorDriver(TIM_HandleTypeDef* timer, uint32_t CH);

	void set_duty(int32_t duty);

	void enable();

	void disable();

};

#endif /* SRC_MOTORDRIVER_H_ */
