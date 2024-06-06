/**
 *
 * @file MotorDriver.cpp
 *
 * @brief Class that controls a motor
 *
 *
 */

#include "MotorDriver.h"

/**
 * @brief Constructor for the MotorDriver class.
 *
 * @param timer A pointer to the timer address.
 * @param FWD The forward direction PWM generation timer channel.
 * @param BCK The backward direction PWM generation timer channel.
 */
MotorDriver::MotorDriver(TIM_HandleTypeDef* timer, uint32_t CH) {
    // TODO Auto-generated constructor stub
    this->timer = timer;
    this->duty = 0;
    this->CH = CH;
}

/**
 * @brief Sets the duty cycle for the motor
 *
 * @param pwm The desired PWM percentage
 */
void MotorDriver::set_duty(int32_t pwm) {
    this->duty = pwm * 2000 / 128; // scale number to our format of 4-bit number

        __HAL_TIM_SET_COMPARE((this->timer), this->CH, this->duty);
}

/**
 * @brief Enables the motor driver by starting the Timer for PWM generation.
 */
void MotorDriver::enable() {
    HAL_TIM_PWM_Start((this->timer), this->CH);
}

/**
 * @brief Disables the motor driver by stopping the Timer for PWM generation .
 */
void MotorDriver::disable() {
    HAL_TIM_PWM_Stop((this->timer), this->CH);
}
