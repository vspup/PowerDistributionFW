/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define COUNT_400V_PULL        3
#define TIME_PRECHARGE_OFF     2000
#define TIME_LED_GREEN_BLINK   500
#define TIME_SWITCH_PRESSHED   1000//200
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
void LED_GreenHandler (void);
void SwitchTimingHandlar(void);
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SWITCH_Pin LL_GPIO_PIN_13
#define SWITCH_GPIO_Port GPIOC
#define DE2_Pin LL_GPIO_PIN_1
#define DE2_GPIO_Port GPIOA
#define RD_EN_Pin LL_GPIO_PIN_2
#define RD_EN_GPIO_Port GPIOB
#define PRECHRG_Pin LL_GPIO_PIN_10
#define PRECHRG_GPIO_Port GPIOB
#define MAINS_REL_Pin LL_GPIO_PIN_11
#define MAINS_REL_GPIO_Port GPIOB
#define CMP_400V_Pin LL_GPIO_PIN_12
#define CMP_400V_GPIO_Port GPIOB
#define SET_SERIAL_Pin LL_GPIO_PIN_13
#define SET_SERIAL_GPIO_Port GPIOB
#define LED_R_Pin LL_GPIO_PIN_7
#define LED_R_GPIO_Port GPIOB
#define LED_G_Pin LL_GPIO_PIN_8
#define LED_G_GPIO_Port GPIOB
#define LED_B_Pin LL_GPIO_PIN_9
#define LED_B_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
