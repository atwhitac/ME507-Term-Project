/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define REV_Pin GPIO_PIN_3
#define REV_GPIO_Port GPIOA
#define FIRE_Pin GPIO_PIN_4
#define FIRE_GPIO_Port GPIOA
#define IN1_1_Pin GPIO_PIN_5
#define IN1_1_GPIO_Port GPIOA
#define INH_1_Pin GPIO_PIN_7
#define INH_1_GPIO_Port GPIOA
#define IN1_2_Pin GPIO_PIN_0
#define IN1_2_GPIO_Port GPIOB
#define IN1_1B12_Pin GPIO_PIN_12
#define IN1_1B12_GPIO_Port GPIOB
#define INH_2_Pin GPIO_PIN_13
#define INH_2_GPIO_Port GPIOB
#define IN2_1_Pin GPIO_PIN_14
#define IN2_1_GPIO_Port GPIOB
#define USER2_Pin GPIO_PIN_15
#define USER2_GPIO_Port GPIOB
#define USER1_Pin GPIO_PIN_8
#define USER1_GPIO_Port GPIOA
#define SAFETY_Pin GPIO_PIN_9
#define SAFETY_GPIO_Port GPIOA
#define BL_Pin GPIO_PIN_5
#define BL_GPIO_Port GPIOB
#define BL_EXTI_IRQn EXTI9_5_IRQn
#define BR_Pin GPIO_PIN_7
#define BR_GPIO_Port GPIOB
#define BR_EXTI_IRQn EXTI9_5_IRQn
#define FL_Pin GPIO_PIN_8
#define FL_GPIO_Port GPIOB
#define FL_EXTI_IRQn EXTI9_5_IRQn
#define FR_Pin GPIO_PIN_9
#define FR_GPIO_Port GPIOB
#define FR_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

void Set_LED (int LEDnum, int Red, int Green, int Blue);

void set_LED_states(int mode, int lgtype);

void clear_LEDs(void);

void Set_Brightness (int brightness);

void WS2812_Send (void);

void LED_Safety(int mode, int lgtype);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
