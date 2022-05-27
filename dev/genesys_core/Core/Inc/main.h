/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32h7xx_hal.h"

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
void console(const char*);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_SPI_MOSI_Pin GPIO_PIN_6
#define LED_SPI_MOSI_GPIO_Port GPIOE
#define user_LED_right_Pin GPIO_PIN_0
#define user_LED_right_GPIO_Port GPIOF
#define user_LED_left_Pin GPIO_PIN_1
#define user_LED_left_GPIO_Port GPIOF
#define analog_in_1_Pin GPIO_PIN_2
#define analog_in_1_GPIO_Port GPIOC
#define analog_in_2_Pin GPIO_PIN_3
#define analog_in_2_GPIO_Port GPIOC
#define UART_TX_Pin GPIO_PIN_0
#define UART_TX_GPIO_Port GPIOA
#define UART_RX_Pin GPIO_PIN_1
#define UART_RX_GPIO_Port GPIOA
#define buzzer_Pin GPIO_PIN_2
#define buzzer_GPIO_Port GPIOA
#define AUDIO_SPI_MOSI_Pin GPIO_PIN_7
#define AUDIO_SPI_MOSI_GPIO_Port GPIOA
#define pb_array_Pin GPIO_PIN_4
#define pb_array_GPIO_Port GPIOC
#define TP17_Pin GPIO_PIN_0
#define TP17_GPIO_Port GPIOG
#define LED_SPI_CLK_Pin GPIO_PIN_12
#define LED_SPI_CLK_GPIO_Port GPIOE
#define LED_SPI_reg_UPD_Pin GPIO_PIN_13
#define LED_SPI_reg_UPD_GPIO_Port GPIOE
#define MIDI_in_Pin GPIO_PIN_12
#define MIDI_in_GPIO_Port GPIOB
#define MIDI_out_Pin GPIO_PIN_13
#define MIDI_out_GPIO_Port GPIOB
#define UART_RTS_Pin GPIO_PIN_14
#define UART_RTS_GPIO_Port GPIOB
#define UART_CTS_Pin GPIO_PIN_15
#define UART_CTS_GPIO_Port GPIOB
#define MUX_sel_A_Pin GPIO_PIN_11
#define MUX_sel_A_GPIO_Port GPIOD
#define MUX_sel_B_Pin GPIO_PIN_12
#define MUX_sel_B_GPIO_Port GPIOD
#define MUX_sel_C_Pin GPIO_PIN_13
#define MUX_sel_C_GPIO_Port GPIOD
#define audio_codec_reset_Pin GPIO_PIN_7
#define audio_codec_reset_GPIO_Port GPIOD
#define audio_SPI_MISO_Pin GPIO_PIN_9
#define audio_SPI_MISO_GPIO_Port GPIOG
#define audio_SPI_MS_Pin GPIO_PIN_10
#define audio_SPI_MS_GPIO_Port GPIOG
#define audio_SPI_MCLK_Pin GPIO_PIN_11
#define audio_SPI_MCLK_GPIO_Port GPIOG
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
