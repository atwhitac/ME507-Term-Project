/**
 * @file RCReceiver.cpp
 *
 * @brief Class that handles signals and triggers for the RC Receiver.
 *
 */

#include "RCReceiver.h"
#include "stm32f4xx_hal.h"

/**
 * @brief Constructor for the RCReceiver class.
 *
 * Initializes the RCReceiver object and starts the input capture interrupt for the given timer and channel.
 *
 * @param timer Pointer to the timer used.
 * @param channel Timer channel used for capturing the RC signal.
 */
RCReceiver::RCReceiver(TIM_HandleTypeDef* timer, uint32_t channel) {
    // Initialize member variables
    this->timer = timer;
    this->channel = channel;
    this->count1 = 0;
    this->count2 = 0;
    this->diff = 0;
    this->First_Capture = 0;
    this->signal = 1500;
    this->flag = 0;
    this->test = 0;

    // Start the input capture interrupt
    HAL_TIM_IC_Start_IT(this->timer, this->channel);
}

/**
 * @brief Triggers the signal capture for the RC receiver.
 *
 * This function is called to handle a direct input capture. It captures the RC signal pulse width and updates the signal value.
 * It also checks to make sure the signal doesn't go out of sync, or to not use a bad signal.
 */
void RCReceiver::trigger_signal() {
    if (this->First_Capture == 0) {
        this->count1 = HAL_TIM_ReadCapturedValue(this->timer, this->channel);
        this->First_Capture = 1;
        this->flag = 0;
    } else { // If the first rising edge is captured, now capture the second edge
        this->count2 = HAL_TIM_ReadCapturedValue(this->timer, this->channel); // Read second value

        if (this->count2 > this->count1) {
            this->diff = this->count2 - this->count1;
        } else if (this->count1 > this->count2) {
            this->diff = (0xffff - this->count1) + this->count2;
        }

        if (this->diff > 3000) {
            this->count1 = this->count2;
        } else {
            this->test = 1;
            this->signal = this->diff; // Save the signal if it is good
            __HAL_TIM_SET_COUNTER(this->timer, 0); // Reset the counter
            this->First_Capture = 0; // Set it back to false
            this->flag = 1;
        }
    }
}

/**
 * @brief Gets the captured signal value.
 *
 * @return The captured signal value.
 */
int16_t RCReceiver::get_signal() {
    return this->signal;
}

/**
 * @brief Checks the flag status.
 *
 * @return The flag status as an integer (true/false).
 */
uint8_t RCReceiver::check_flag() {
    return flag;
}

/**
 * @brief Sets the flag status.
 *
 * @param n An integer n to set the flag true/false.
 */
void RCReceiver::set_flag(uint8_t n) {
    this->flag = n;
}
