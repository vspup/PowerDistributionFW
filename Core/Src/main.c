/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
// version 1.0.1
#define N_LOOP_CYCLES     (5)   // cyles
#define CYCLES_TIME_ON    20  //  sec
#define CYCLES_TIME_OFF   40  //  sec
#define TIME_PRECHARGE_OFF     5000  // msec

#define   TIME_POWER_OFF    30000
#define   TIME_POWER_ON     10000
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#include <stdio.h>
#include <string.h>
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
uint32_t counterSwitch = 0;
uint8_t stateSwitch = 0;
uint8_t leaveSwitch = 0;
uint32_t timeoutPwrOff = 0;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void SwitchTimingHandlar(void)
{
	timeoutPwrOff ++;
	if(leaveSwitch)
	{
		return;
	}
	counterSwitch ++;
}

uint8_t greenLEDState = 0;
uint32_t greenLedTimer = 0;
void LED_GreenHandler (void)
{


	if(greenLEDState == 0)
	{
	  return; //	LL_GPIO_ResetOutputPin(LED_G_GPIO_Port, LED_G_Pin);
	}

	if(greenLEDState == 1)
	{
		greenLedTimer++;

		if(greenLedTimer > TIME_LED_GREEN_BLINK)
		{
			greenLedTimer = 0;
			LL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
		}
	}

	if(greenLEDState == 2)
	{
		LL_GPIO_SetOutputPin(LED_G_GPIO_Port, LED_G_Pin);
	}
}
uint8_t cntPwrOff = 0;
uint32_t loop_cnt = 0;
uint8_t rx_byte;
uint8_t rx_buff[255]={0};
uint8_t rx_cnt = 0;
uint8_t get_cmd = 0;
int cnt_reboot= 0;

#define CMD_NUM 5
char cmd_list[CMD_NUM][10]  = {"off",
                               "bus",
				               "reboot",
                               "on",
                               "count"};


int i = 0;
uint8_t power_state = 0;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(rx_byte == 0x0D)
  {
	 for(i = 0; i < CMD_NUM; i++)
	 {
		 char *ptData = NULL;
		 ptData = strstr((char*)&rx_buff[0], (char*)&cmd_list[i][0]);
		 if(ptData != NULL)
		 {
			 get_cmd = i + 1;
			 break;
		 }
	 }
	 /*if(get_cmd == CMD_NUM)
	 {
		get_cmd = 0;
		char tempBuff[32] = {0};
		sprintf(tempBuff, "count:%d;", cnt_reboot);
		Send_RS485_Data(tempBuff);

	 }*/
	 memset((char*)&rx_buff[0], 0x00, sizeof(rx_buff));
	 rx_cnt = 0;
  }
  else
  {
	  rx_buff[rx_cnt++] = rx_byte;
  }
  HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
}

void PowerOn (void)
{
	greenLEDState = 2;
	LL_GPIO_SetOutputPin(MAINS_REL_GPIO_Port, MAINS_REL_Pin);
	HAL_Delay(TIME_PRECHARGE_OFF);
	LL_GPIO_SetOutputPin(PRECHRG_GPIO_Port, PRECHRG_Pin);
	power_state = 1;
}

void PowerOff (void)
{
	LL_GPIO_ResetOutputPin(MAINS_REL_GPIO_Port, MAINS_REL_Pin);
	HAL_Delay(TIME_PRECHARGE_OFF);
	LL_GPIO_ResetOutputPin(PRECHRG_GPIO_Port, PRECHRG_Pin);
	greenLEDState = 1;
	power_state = 0;
}

void Send_RS485_Data (char *buff)
{
	uint8_t len = strlen(buff);
	LL_GPIO_SetOutputPin(DE2_GPIO_Port, DE2_Pin);
	HAL_Delay(1);
	HAL_UART_Transmit(&huart2, (uint8_t*)buff, len, len *2);
	HAL_Delay(1);
	LL_GPIO_ResetOutputPin(DE2_GPIO_Port, DE2_Pin);
}
uint8_t reboot_flag = 0;
uint8_t reboot_state_off =0;
uint32_t reboot_time = 0;
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
  HAL_Delay(1000);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
  /* USER CODE BEGIN 2 */
  uint8_t cnt400V[2] = {0};
  while(1)
  {
	  if(LL_GPIO_IsInputPinSet(CMP_400V_GPIO_Port,CMP_400V_Pin))
	  {
		  cnt400V[0]++;
		  cnt400V[1]=0;
	  }
	  else
	  {
		  cnt400V[1]++;
		  cnt400V[0]=0;
	  }
	  HAL_Delay(33);

	  if(cnt400V[1] >= COUNT_400V_PULL)
	  {
	       LL_GPIO_SetOutputPin(SET_SERIAL_GPIO_Port, SET_SERIAL_Pin);
           LL_GPIO_SetOutputPin(LED_R_GPIO_Port, LED_R_Pin);
		  break;
	  }
	  else if(cnt400V[0] >= COUNT_400V_PULL)
	  {
		  LL_GPIO_ResetOutputPin(SET_SERIAL_GPIO_Port, SET_SERIAL_Pin);
		  LL_GPIO_SetOutputPin(LED_B_GPIO_Port, LED_B_Pin);
		  break;
	  }

  }
  //LL_GPIO_SetOutputPin(MAINS_REL_GPIO_Port, MAINS_REL_Pin);
  HAL_Delay(2000);
  //LL_GPIO_SetOutputPin(PRECHRG_GPIO_Port, PRECHRG_Pin);
  greenLEDState = 1;
  LL_GPIO_ResetOutputPin(LED_R_GPIO_Port, LED_R_Pin);
  LL_GPIO_ResetOutputPin(LED_G_GPIO_Port, LED_G_Pin);
  LL_GPIO_ResetOutputPin(LED_B_GPIO_Port, LED_B_Pin);
  counterSwitch = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

 /* while(LL_GPIO_IsInputPinSet(SWITCH_GPIO_Port,SWITCH_Pin) != 0)
  {

  }*/

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	if(LL_GPIO_IsInputPinSet(SWITCH_GPIO_Port,SWITCH_Pin) == 0)
	{
		if(LL_GPIO_IsInputPinSet(RD_EN_GPIO_Port, RD_EN_Pin) == 0)
		{
		   while(LL_GPIO_IsInputPinSet(RD_EN_GPIO_Port,RD_EN_Pin) == 0)
		   {
			  LL_GPIO_SetOutputPin(LED_R_GPIO_Port, LED_R_Pin);
			  LL_GPIO_ResetOutputPin(LED_G_GPIO_Port, LED_G_Pin);
			  LL_GPIO_ResetOutputPin(LED_B_GPIO_Port, LED_B_Pin);
		   }

			  LL_GPIO_ResetOutputPin(LED_R_GPIO_Port, LED_R_Pin);
			  LL_GPIO_ResetOutputPin(LED_G_GPIO_Port, LED_G_Pin);
			  LL_GPIO_ResetOutputPin(LED_B_GPIO_Port, LED_B_Pin);
			  counterSwitch = 0;
			  leaveSwitch   = 0;

		}

		if( (counterSwitch > TIME_SWITCH_PRESSHED)&&(leaveSwitch == 0) )
		{
			stateSwitch ^= 1;

			if(stateSwitch)
			{
				PowerOn ();

				//HAL_Delay(CYCLES_TIME_ON  * 1000);
			}
			else
			{
				cnt_reboot = 0;
				PowerOff ();

				//HAL_Delay(CYCLES_TIME_OFF  * 1000);
			}
			leaveSwitch = 1;
		 }
	  }
	  else
	  {
		counterSwitch = 0;
		leaveSwitch   = 0;
	  }

	if(get_cmd)
	{
		switch(get_cmd)
		{

			case 1:
				if(power_state)
				{
					if(LL_GPIO_IsInputPinSet(RD_EN_GPIO_Port, RD_EN_Pin) == 0)
					{
						Send_RS485_Data("power_off:-1;");
					}
					else
					{
						Send_RS485_Data("power_off:0;");
						HAL_Delay(30000);
						PowerOff ();
					}
				}
				else
				{
					Send_RS485_Data("power_off:1;");
				}
			break;
			case 2:
				if(LL_GPIO_IsInputPinSet(RD_EN_GPIO_Port, RD_EN_Pin) == 0)
				{
					Send_RS485_Data("bus:1;");
				}
				else
				{
					Send_RS485_Data("bus:0;");
				}
			break;
			case 3:
				if(LL_GPIO_IsInputPinSet(RD_EN_GPIO_Port, RD_EN_Pin) == 0)
				{
					Send_RS485_Data("reboot:-1;");
				}
				else
				{
					if(reboot_flag)
					{
					    Send_RS485_Data("reboot:2;");
					}
					else
					{
						reboot_flag = 1;
						reboot_state_off = 1;
						reboot_time = HAL_GetTick();
						cnt_reboot ++;
						Send_RS485_Data("reboot:1;");
					}
				}
			break;
			case 4:
				Send_RS485_Data("power_on:1;");
				PowerOn ();
			break;
			case 5:
				char tempBuff[32] = {0};
				sprintf(tempBuff, "count:%d;", cnt_reboot);
				Send_RS485_Data(tempBuff);
			break;

		}
		get_cmd = 0;
     }

	if(reboot_flag)
	{
	   if(reboot_state_off)
	   {
		   if(HAL_GetTick() > reboot_time + TIME_POWER_OFF)
		   {
			   reboot_state_off = 0;
			   PowerOff ();
			   reboot_time = HAL_GetTick();
		   }
	   }
	   else
	   {
		   if(HAL_GetTick() > reboot_time + TIME_POWER_ON)
		   {
			   PowerOn ();
			   reboot_flag = 0;
		   }
	   }
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
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
  }

  /* HSI configuration and activation */
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 8, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

  /* Set AHB prescaler*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* Sysclk activation on the main PLL */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Set APB1 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(64000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOF);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_ResetOutputPin(DE2_GPIO_Port, DE2_Pin);

  /**/
  LL_GPIO_ResetOutputPin(PRECHRG_GPIO_Port, PRECHRG_Pin);

  /**/
  LL_GPIO_ResetOutputPin(MAINS_REL_GPIO_Port, MAINS_REL_Pin);

  /**/
  LL_GPIO_ResetOutputPin(SET_SERIAL_GPIO_Port, SET_SERIAL_Pin);

  /**/
  LL_GPIO_ResetOutputPin(LED_R_GPIO_Port, LED_R_Pin);

  /**/
  LL_GPIO_ResetOutputPin(LED_G_GPIO_Port, LED_G_Pin);

  /**/
  LL_GPIO_ResetOutputPin(LED_B_GPIO_Port, LED_B_Pin);

  /**/
  GPIO_InitStruct.Pin = SWITCH_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(SWITCH_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = DE2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(DE2_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = RD_EN_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(RD_EN_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = PRECHRG_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(PRECHRG_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = MAINS_REL_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(MAINS_REL_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = CMP_400V_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(CMP_400V_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = SET_SERIAL_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(SET_SERIAL_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED_R_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED_R_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED_G_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED_G_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED_B_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED_B_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
