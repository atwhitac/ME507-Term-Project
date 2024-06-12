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
#include "RCReceiver.h"
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

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
DMA_HandleTypeDef hdma_tim3_ch1_trig;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

// LED variables
#define MAX_LED 12
#define USE_BRIGHTNESS 0
uint8_t LED_Data[MAX_LED][4];
uint8_t LED_Mod[MAX_LED][4];
uint16_t pwmData[(24*MAX_LED)+50];
volatile int datasentflag = 1;


// Motor variables
uint8_t startup = 0;
uint8_t rev_flag = 0;
uint16_t rev_tick = 0;


// Shoot variables
uint16_t dart_counter = 0;
uint16_t idx = 1;
uint8_t last_fired = 0;


// Mode variables
// Timer that updates every while loop iteration
uint16_t curr_tick;
// Handles trigger logic=
uint8_t trigger_tick = 0;
uint8_t trigger_flag = 0;
// Handles shooting logic
uint16_t shoot_flag = 0;
uint16_t shoot_state = 4; // Start in safety
uint16_t shoot_tick = 0;
// Handles mode selection
uint8_t mode_state = 0;
uint8_t mode_flag = 0;
uint16_t mode_tick = 0;
uint8_t left_state = 0;
uint8_t alt_state = 0;
uint8_t both_state = 0;
uint8_t right_state = 0;
// Timing for each solenoid
uint16_t left_tick;
uint16_t right_tick;


// ADC variables
uint16_t adc_value_9;
uint16_t adc_value_2;
uint16_t Fire_Rate;
uint16_t counts = 1600;


// User interface variables
// Handles type selection
uint8_t type = 0;
uint8_t type_tick = 0;
uint16_t type_list[] = {1,4,500}; // Dart capacities for each type
uint8_t type_flag = 0;


// RCReceiver object
RCReceiver receiver1(&htim1, TIM_CHANNEL_3);
int16_t signal1 = 1500;


// UART Message for Debugging
char msg[100];


// ADC Channel selection functions
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


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);
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
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  // Initializations
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3);
  HAL_ADC_Start(&hadc1);
  HAL_ADC_Init(&hadc1);


  // Set the solenoid pin states
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);


  // Disable solenoids to start
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);


  // Initialize LEDs in safety mode
  LED_Safety(mode_state,type);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  // RC Receiver E-Stop
	  signal1 = receiver1.get_signal();
	  if ((signal1 > 1600 || signal1 < 1400)){
		  shoot_state = 4; // go to safety state if RC receiver triggered
		  LED_Safety(mode_state, type);
	  }

	  // Get time elapsed every loop
	  curr_tick = HAL_GetTick();

//	  // UART debugging
//	  snprintf(msg, sizeof(msg), "BL: %i BR: %i FL: %i FR: %i  \n\r",!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5), !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7),
//			  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	   !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8), !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9));
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
	  if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) && (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9) && !(shoot_state == 4))){
		  // Rev the motors
		  		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1, counts);
		  		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2, counts);
//		  		trigger_tick = curr_tick;
	  }

//	  else if(trigger_flag == 1 && (curr_tick-trigger_tick <1000)){
//		  // Set motors to neutral state
//		  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1, counts);
//		  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2, counts);
//
//	  }
	  else{
		  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1, 1000);
		  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2, 1000);
//		  trigger_flag = 0;
	  }



	  // Setting Types (Single, Burst, Auto)
	  if (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8) && type_flag == 0 && HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9)) {
		  type_flag = 1;
		  type_tick = curr_tick;
		  type++;
		  set_LED_states(mode_state, type);
		  idx = type_list[type%3];

		  }
	  else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8) && (curr_tick - type_tick) > 250 && type_flag ==1){ //
		  type_flag = 0;

	  }

	  // Setting Modes (L, Both, Alt, R)
	  if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) && mode_flag == 0 && HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9)) {
		  mode_flag = 1;
		  mode_tick = curr_tick;

		  if (mode_state == 3){
			  mode_state = 0;
		  }
		  else{
			  mode_state++;
		  }
		  set_LED_states(mode_state, type);
		  }
	  else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) && (curr_tick - mode_tick) > 250 && mode_flag == 1){
		  mode_flag = 0;
	  }


	  // Trigger State Machine
	  // Off state
	  if (shoot_state == 0){
		  if (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9)){
			  shoot_state = 4;
			  LED_Safety(mode_state, type);
		  }
		  if (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3) && HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9)){
			shoot_state = 1;
			shoot_flag = 1;
			shoot_tick = curr_tick;
		  }
	  }

	  // Rising Edge State
	  else if(shoot_state == 1){
		  if (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9)){
			  shoot_state = 4;
			  LED_Safety(mode_state, type);
		  }
		  else if (curr_tick - shoot_tick > 10){
			  shoot_state = 2;
		  }
	  }

	  // On state
	  else if(shoot_state == 2){
		  if (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9)){
			  shoot_state = 4;
			  LED_Safety(mode_state, type);
		  }
		  else if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)){
			  shoot_state = 3;
			  shoot_flag = 0;
			  shoot_tick = curr_tick;
		  }
	  }
	  // Falling Edge state
	  else if(shoot_state == 3) {
		  if (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9)){
			  shoot_state = 4;
			  LED_Safety(mode_state, type);
		  }
		  else if (curr_tick - shoot_tick > 10){
			  shoot_state = 0;
			  rev_tick = curr_tick;
		  }
	  }
	  // Safety State
	  else if(shoot_state == 4){
		  shoot_flag = 0;
		  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1, 1000);
		  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2, 1000);
		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_RESET);
		  if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9) && HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3) && (signal1 < 1600 && signal1 > 1400)){
			  shoot_state = 0;
			  set_LED_states(mode_state, type);
		  }

	  }




	  // Fire Trigger Logic
	  if (shoot_flag){
//		  trigger_tick = curr_tick;
		  // Left State
		  if (mode_state == 0){

			  if (left_state == 0){
				  left_tick = curr_tick;
				  left_state = 1;

			  }
			  else if(left_state == 1){

				  if ((dart_counter < idx) && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5) && ((curr_tick - left_tick) > Fire_Rate+10)){
					  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_SET);
					  left_tick = curr_tick;
					  dart_counter += 1;
				  }
				  else if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)){
					  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_RESET);

				  }
			  }
		  }

		  // Alt State
		  else if (mode_state == 1){
			  if (alt_state == 0){
				  left_tick = curr_tick;
				  right_tick = curr_tick;
				  if (last_fired == 0) {
						  alt_state = 1;
					  }

				  else{
						  alt_state = 2;
					  }

			  }


			  else if (alt_state == 1){

				  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_RESET);
				  last_fired = 1;

				  if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8) && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7) && (curr_tick-left_tick > Fire_Rate)){
					  dart_counter++;

					  if (dart_counter == idx){
						  alt_state = 3;
					  }
					  else{
					  alt_state = 2;
					  right_tick = curr_tick;

					  }
				  }

			  }

			  else if (alt_state == 2){
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_RESET);
				  last_fired = 0;

				  if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5) && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9) && (curr_tick - right_tick > Fire_Rate)){
					  dart_counter ++;

					  if (dart_counter == idx){
						  alt_state = 3;
					  }
					  else{
					  alt_state = 1;
					  left_tick = curr_tick;
					  }
				  }


			  }

			  else if (alt_state == 3){
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_RESET);

			  }

		  }


		  // Both State
		  else if (mode_state == 2){

			  if (both_state == 0){
				  left_tick = curr_tick;
				  both_state = 1;

			  }

			  else if (both_state == 1){


					  if (dart_counter < (idx) && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7) && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5) && (curr_tick-left_tick) > (Fire_Rate+10)){
						  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_SET);
						  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_SET);
						  left_tick = curr_tick;
						  dart_counter += 2;
					  }

					  else if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9) && !HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)){
						  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_RESET);
						  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_RESET);
					  }

			  }


		  }

		  // Right
		  else if (mode_state == 3){

			  if (right_state == 0){
				  right_tick = curr_tick;
				  right_state = 1;
			  }

			  else if (right_state == 1){

				  if ((dart_counter < idx) &&!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7) && (curr_tick - right_tick) > Fire_Rate+10){
					  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_SET);
					  right_tick = curr_tick;
					  dart_counter += 1;
				  }
				  else if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)){
					  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_RESET);

				  }
			  }
		  }


	  }

	  else{


		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_RESET);
		  dart_counter = 0;
		  alt_state = 0;
		  both_state = 0;
		  left_state = 0;
		  right_state = 0;

		  ADC_Select_CH2();
		  HAL_ADC_Start(&hadc1);
		  HAL_ADC_PollForConversion(&hadc1,50);
		  adc_value_2 = HAL_ADC_GetValue(&hadc1); // Right POT
		  HAL_ADC_Stop(&hadc1);

		  counts = adc_value_2*(1500-1000)/(4096 - 0) + 1000;

		  ADC_Select_CH9();
		  HAL_ADC_Start(&hadc1);
		  HAL_ADC_PollForConversion(&hadc1,50);
		  adc_value_9 = HAL_ADC_GetValue(&hadc1); // Left POT
		  HAL_ADC_Stop(&hadc1);

		  Fire_Rate = (4096-adc_value_9)*100/4096;


	  }




  }
  /* USER CODE END 3 */
}

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
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 95;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

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
  htim3.Init.Period = 119;
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

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
  HAL_GPIO_WritePin(GPIOA, IN1_1_Pin|INH_1_Pin|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, IN1_2_Pin|IN1_1B12_Pin|INH_2_Pin|IN2_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : REV_Pin FIRE_Pin USER1_Pin SAFETY_Pin */
  GPIO_InitStruct.Pin = REV_Pin|FIRE_Pin|USER1_Pin|SAFETY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : IN1_1_Pin INH_1_Pin PA11 PA12 */
  GPIO_InitStruct.Pin = IN1_1_Pin|INH_1_Pin|GPIO_PIN_11|GPIO_PIN_12;
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

  /*Configure GPIO pin : USER2_Pin */
  GPIO_InitStruct.Pin = USER2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(USER2_GPIO_Port, &GPIO_InitStruct);

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

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){



	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3){
		receiver1.trigger_signal();
	}


}

void Set_LED (int LEDnum, int Red, int Green, int Blue)
{
	LED_Data[LEDnum][0] = LEDnum;
	LED_Data[LEDnum][1] = Green;
	LED_Data[LEDnum][2] = Red;
	LED_Data[LEDnum][3] = Blue;
}

void clear_LEDs(void)
{
	for (int i=0; i<MAX_LED; i++)
	{
		Set_LED(i,0,0,0);
	}
}

void set_LED_states(int mode, int lgtype)
{

	// Mode Selection
	int r1 = 63;
	int g1 = 63;
	int b1 = 63;
	// Background

    // Orange
	int r2 = 77;
	int g2 = 30;
	int b2 = 5;


//	// Blue
//	int r2 = 0;
//	int g2 = 0;
//	int b2 = 15;



	// determine which fire type is active (single, burst, auto)
	int firetype = lgtype%3;

	//LED matrix follows an S pattern starting from right to left
	//Must do some funky conversion to match the firemode/type grid on the blaster

	clear_LEDs();

	// Row 1
	if(firetype == 0)
	{
		for(int i=0; i<4; i++)
		{
			Set_LED(3-i, r2, g2, b2);
		}

		Set_LED(3-mode, r1, g1, b1);

		Set_LED(4 + mode, r2, g2, b2);
		Set_LED(11 - mode, r2, g2, b2);



	}
	else if (firetype == 1)
	{
	// Row 2
		for(int i=0; i<4; i++)
		{
			Set_LED(4+i, r2, g2, b2);

		}

		Set_LED(4 + mode, r1, g1, b1);

		Set_LED(3-mode, r2, g2, b2);
		Set_LED(11 - mode, r2, g2, b2);


	}
	else if (firetype == 2)
	{
	// Row 3
		for(int i=0; i<4; i++)
		{
			Set_LED(11-i, r2, g2, b2);
		}

		Set_LED(11 - mode, r1, g1, b1);

		Set_LED(4 + mode, r2, g2, b2);
		Set_LED(3-mode, r2, g2, b2);

	}

	WS2812_Send();
}

#define PI 3.14159265

void Set_Brightness (int brightness)  // 0-45
{
	#if USE_BRIGHTNESS

		if (brightness > 45) brightness = 45;
		for (int i=0; i<MAX_LED; i++)
		{
			LED_Mod[i][0] = LED_Data[i][0];
			for (int j=1; j<4; j++)
			{
				float angle = 90-brightness;  // in degrees
				angle = angle*PI / 180;  // in rad
				LED_Mod[i][j] = (LED_Data[i][j])/(tan(angle));
			}
		}

	#endif
}

void LED_Safety(int mode, int lgtype){

	int firetype = lgtype%3;
	int r2 = 63;
	int g2 = 0;
	int b2 = 0;

	if(firetype == 0)
		{
			for(int i=0; i<4; i++)
			{
				Set_LED(3-i, r2, g2, b2);
			}


			Set_LED(4 + mode, r2, g2, b2);
			Set_LED(11 - mode, r2, g2, b2);



		}
		else if (firetype == 1)
		{
		// Row 2
			for(int i=0; i<4; i++)
			{
				Set_LED(4+i, r2, g2, b2);
			}

			Set_LED(3-mode, r2, g2, b2);
			Set_LED(11 - mode, r2, g2, b2);


		}
		else if (firetype == 2)
		{
		// Row 3
			for(int i=0; i<4; i++)
			{
				Set_LED(11-i, r2, g2, b2);
			}

			Set_LED(4 + mode, r2, g2, b2);
			Set_LED(3-mode, r2, g2, b2);

		}

		WS2812_Send();


}




void WS2812_Send (void)
{
	uint32_t indx=0;
	uint32_t color;


	for (int i= 0; i<MAX_LED; i++)
	{
		#if USE_BRIGHTNESS
				color = ((LED_Mod[i][1]<<16) | (LED_Mod[i][2]<<8) | (LED_Mod[i][3]));
		#else
				color = ((LED_Data[i][1]<<16) | (LED_Data[i][2]<<8) | (LED_Data[i][3]));
		#endif

		for (int i=23; i>=0; i--)
		{
			if (color&(1<<i))
			{
				pwmData[indx] = 80;  // 2/3 of 120
			}

			else pwmData[indx] = 40;  // 1/3 of 120

			indx++;
		}

	}

	for (int i=0; i<50; i++)
	{
		pwmData[indx] = 0;
		indx++;
	}

	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)pwmData, indx);
	while (!datasentflag){};
	datasentflag = 0;
}



void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
	datasentflag=1;
}

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
