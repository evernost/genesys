// ============================================================================
// Module name     : pcm3168a 
// File name       : pcm3168a.h
// Purpose         : TI PCM3168A ADC/DAC low level driver.
// Author          : Quentin Biache
// Creation date   : 17th October, 2021
// ============================================================================

// Define to prevent recursive inclusion --------------------------------------
#ifndef __PCM3168A_H
#define __PCM3168A_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes -------------------------------------------------------------------
#include "stm32h7xx_hal.h"

// Defines --------------------------------------------------------------------
#define REG_ADDR_RESET_CONTROL            0x40
#define REG_ADDR_DAC_CONTROL_1            0x41
#define REG_ADDR_DAC_CONTROL_2            0x42
#define REG_ADDR_DAC_OUTPUT_PHASE         0x43
#define REG_ADDR_DAC_SOFT_MUTE_CONTROL    0x44
#define REG_ADDR_DAC_ZERO_FLAG            0x45
#define REG_ADDR_DAC_CONTROL_3            0x46
#define REG_ADDR_DAC_ATTENUATION          0x47
#define REG_ADDR_ADC_SAMPLING_MODE        0x50
#define REG_ADDR_ADC_CONTROL_1            0x51
#define REG_ADDR_ADC_CONTROL_2            0x52
#define REG_ADDR_ADC_INPUT_CONFIGURATION  0x53
#define REG_ADDR_ADC_INPUT_PHASE          0x54
#define REG_ADDR_ADC_SOFT_MUTE            0x55
#define REG_ADDR_ADC_OVERFLOW_FLAG        0x56
#define REG_ADDR_ADC_CONTROL_3            0x57
#define REG_ADDR_ADC_ATTENUATION          0x58

#define READ_MASK 0x80


// Types ----------------------------------------------------------------------
typedef struct pcm3168a_device
{
  uint8_t reset_control;
  uint8_t dac_control_1;
  uint8_t dac_control_2;
  uint8_t dac_output_phase;
  uint8_t dac_soft_mute_control;
  uint8_t dac_zero_flag;
  uint8_t dac_control_3;
  uint8_t dac_attenuation[8];
  uint8_t adc_sampling_mode;
  uint8_t adc_control_1;
  uint8_t adc_control_2;
  uint8_t adc_input_configuration;
  uint8_t adc_input_phase;
  uint8_t adc_soft_mute;
  uint8_t adc_overflow_flag;
  uint8_t adc_control_3;
  uint8_t adc_attenuation[6];
} pcm3168a_device;



/* Exported functions prototypes ---------------------------------------------*/
void pcm3168a_init(pcm3168a_device*);
void pcm3168a_read(pcm3168a_device*);
void pcm3168a_write(pcm3168a_device*);
void pcm3168a_write_reg(uint8_t, uint8_t);
uint8_t pcm3168a_read_reg(uint8_t);


#ifdef __cplusplus
}
#endif

#endif // __PCM3168A_H


