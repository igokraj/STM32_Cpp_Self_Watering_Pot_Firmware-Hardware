/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Start_Button_Pin GPIO_PIN_13
#define Start_Button_GPIO_Port GPIOC
#define Start_Button_EXTI_IRQn EXTI15_10_IRQn
#define ADC1_Hum_Pin GPIO_PIN_0
#define ADC1_Hum_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define Blue_LED_Pin GPIO_PIN_7
#define Blue_LED_GPIO_Port GPIOC
#define Red_LED_Pin GPIO_PIN_8
#define Red_LED_GPIO_Port GPIOC
#define Buzzer_Status_Pin GPIO_PIN_9
#define Buzzer_Status_GPIO_Port GPIOC
#define Green_LED_Pin GPIO_PIN_8
#define Green_LED_GPIO_Port GPIOA
#define Pump_on_Pin GPIO_PIN_9
#define Pump_on_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define Button_Plus_Pin GPIO_PIN_4
#define Button_Plus_GPIO_Port GPIOB
#define Button_Minus_Pin GPIO_PIN_5
#define Button_Minus_GPIO_Port GPIOB
#define Water_level_Pin GPIO_PIN_6
#define Water_level_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
