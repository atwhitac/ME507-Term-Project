/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "MotorDriver.h"
#include "stm32f4xx_hal.h"
#include <cstdio>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
int16_t duty = 0;
uint8_t flag = 0;
uint16_t counts = 1600;
uint8_t rev_flag = 0;
uint8_t fire_flag = 0;
uint8_t startup = 0;
uint8_t BL = 0;
uint8_t BR = 0;
uint8_t FL = 0;
uint8_t FR = 0;
uint16_t curr_tick;
uint16_t left_tick;
uint16_t right_tick;
uint8_t L_flag = 0;
uint8_t R_flag = 0;
//uint16_t adc_value[2];
//uint8_t adcIdx;
uint16_t adc_value_9;
uint16_t adc_value_2;
uint8_t state = 0;
uint16_t Fire_Rate;
uint16_t dart_counter = 0;
static void ADC_Select_CH9 (void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	  */
	  sConfig.Channel = ADC_CHANNEL_9;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

static void ADC_Select_CH2 (void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	  */
	  sConfig.Channel = ADC_CHANNEL_2;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}
// UART Message for Debugging
char msg[100];
char msg2[100];

enum MODE {
	// Syntax follows: MODE_MAGSELECTION
	SAFETY,
	SINGLE_LEFT,
	SINGLE_RIGHT,
	SINGLE_ALT,
	SINGLE_BOTH,
	BURST_LEFT,
	BURST_RIGHT,
	BURST_ALT,
	BURST_BOTH,
	AUTO_LEFT,
	AUTO_RIGHT,
	AUTO_ALT,
	AUTO_BOTH,
};

// Beam Brake Flags


MotorDriver motor1(&htim2, TIM_CHANNEL_1);
MotorDriver motor2(&htim2, TIM_CHANNEL_2);


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

//  HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(ADC_IRQn);

  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
  HAL_ADC_Start(&hadc1);
  HAL_ADC_Init(&hadc1);

  motor1.enable();
  motor2.enable();

  motor1.set_duty(0);
  motor2.set_duty(0);


  // Solenoid 1 Pins
//  HAL_GPIO_Init(GPIOA,3); // IN1_1
//  HAL_GPIO_Init(GPIOA,7); // INH_1
//  HAL_GPIO_Init(GPIOB,0); // IN1_2
//  // Solenoid 2 Pins
//  HAL_GPIO_Init(GPIOA,10); // IN2_1
//  HAL_GPIO_Init(GPIOA,11); // INH_2
//  HAL_GPIO_Init(GPIOA,12); // IN2_2
  // Button Pin
//  HAL_GPIO_Init(GPIOA,GPIO_PIN_0);

  // Set the solenoid pin states
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);

  // Disable solenoids to start
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);


//  MODE MODE = SAFETY;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */



	  curr_tick = HAL_GetTick();

	  ADC_Select_CH2();
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1,50);
	  adc_value_2 = HAL_ADC_GetValue(&hadc1); // Right POT
	  HAL_ADC_Stop(&hadc1);

	  counts = adc_value_2*(2000-1500)/(4096 - 0) + 1500;

	  ADC_Select_CH9();
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1,50);
	  adc_value_9 = HAL_ADC_GetValue(&hadc1); // Left POT
	  HAL_ADC_Stop(&hadc1);

	  Fire_Rate = (4096-adc_value_9)*100/4096;

//	  snprintf(msg, sizeof(msg), "ADC1: %i ADC2: %i \n\r",adc_value_2, adc_value_9);
//	  HAL_UART_Transmit(&huart1, (uint8_t*)msg, 100,500);
//	  memset(msg,0, sizeof msg);

	  // ESC Startup Procedure
	  if (!startup) {
		  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1, 1000);
		  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2, 1000);
		  HAL_Delay(3000);

		  startup = 1;
	  }


	  // Rev Trigger Logic
	  if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) && HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9)){
		  // Rev the motors
		  		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1, counts);
		  		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2, counts);
	  }
	  else{
		  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1, 1000);
		  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2, 1000);
	  }



	  // Fire Trigger Logic
	  if (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3) && HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9)){

		  // Safety Switch
//		  if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9)){
//			  state = 0;
//		  }
//		  else{
//			  state = 1;
//		  }

			  //first trigger
//			  if (state == 0){
//			  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_RESET);
//				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_RESET);
//			  }

//			  if (state == 1){
//				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_RESET);
//				  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_SET);
//				  if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5) && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)){
//				  	state = 2;
//				  }
//
//			  }
//
//			  else if (state == 2){
//				  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_RESET);
//				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_SET);
//
//				  if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8) && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)){
//				  state = 1;
//				  }
//			  }



//			  // LEFT_SINGLE
//			  if ((curr_tick - left_tick) > Fire_Rate){
//				  if (!L_flag && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)){
//					  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_SET);
//					  L_flag = 1;
//				  }
//				  else if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)){
//					  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_RESET);
//				  }
//
//			  }


//			  // RIGHT_SINGLE
//			  if ((curr_tick - right_tick) > Fire_Rate){
//				  if (!R_flag && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)){
//					  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_SET);
//					  R_flag = 1;
//				  }
//				  else if(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)){
//					  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_RESET);
//				  }
//			  }



//		  	  	  // ALT_AUTO
//		  		  // Use L_flag as a first state
//
//		  		  if (!L_flag && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)){
//		  			  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_SET);
//		  			  L_flag = 1;
//		  			  left_tick = curr_tick;
//		  		  }
//		  		  else if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8) && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7) && (curr_tick-left_tick > Fire_Rate)) {
//		  			  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_SET);
//		  			  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_RESET);
//		  			  right_tick = curr_tick;
//
//		  		  }
//
//		  		  else if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5) && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9) && (curr_tick - right_tick > Fire_Rate)){
//		  			  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_SET);
//		  			  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_RESET);
//		  			  left_tick = curr_tick;
//		  		  }


		  	  // AUTO_BOTH
		  	  if ((curr_tick-left_tick) > Fire_Rate){
		  		  if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7) && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)){
		  			  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_SET);
		  			  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_SET);
		  			  left_tick = curr_tick;

		  		  }
		  		  else if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9) && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)){
		  			  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_RESET);
		  			  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_RESET);

		  		  }
		  	  }

		  }

	  else{
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_RESET);
		  R_flag = 0;
		  L_flag = 0;
	  }



	  }

  }
  /* USER CODE END 3 */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */


  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 95;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 19999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1999;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, IN1_1_Pin|INH_1_Pin|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, IN1_2_Pin|IN1_1B12_Pin|INH_2_Pin|IN2_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : REV_Pin FIRE_Pin SAFETY_Pin */
  GPIO_InitStruct.Pin = REV_Pin|FIRE_Pin|SAFETY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : IN1_1_Pin INH_1_Pin PA10 PA11
                           PA12 */
  GPIO_InitStruct.Pin = IN1_1_Pin|INH_1_Pin|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : IN1_2_Pin IN1_1B12_Pin INH_2_Pin IN2_1_Pin */
  GPIO_InitStruct.Pin = IN1_2_Pin|IN1_1B12_Pin|INH_2_Pin|IN2_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BL_Pin FL_Pin FR_Pin */
  GPIO_InitStruct.Pin = BL_Pin|FL_Pin|FR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BR_Pin */
  GPIO_InitStruct.Pin = BR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BR_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


//void ADC_IRQHandler(void)
//{
//
//  HAL_ADC_IRQHandler(&hadc1); // Call HAL ADC IRQ handler
//}
//
//void HAL_ADC_ConvtCpltCallback (ADC_HandleTypeDef *hadc) {
//  adc_value[adcIdx] = HAL_ADC_GetValue (&hadc1);
//
//  adcIdx = (adcIdx + 1) % 3;
//}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
