// ============================================================================
// Module name     : pcm3168a 
// File name       : pcm3168a.c
// Purpose         : TI PCM3168A ADC/DAC low level driver.
// Author          : Quentin Biache
// Creation date   : 17th October, 2021
// ============================================================================

// ============================================================================
// Description
// ============================================================================
// TODO.

// ============================================================================
// Includes
// ============================================================================
#include "pcm3168a.h"
#include "spi.h"

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
void pcm3168a_init(pcm3168a_device* h_pcm3168a)
{
  uint8_t k;
  
  h_pcm3168a->reset_control           = 0xC0;
  h_pcm3168a->dac_control_1           = 0x00;
  h_pcm3168a->dac_control_2           = 0x00;
  h_pcm3168a->dac_output_phase        = 0x00;
  h_pcm3168a->dac_soft_mute_control   = 0x00;
  h_pcm3168a->dac_zero_flag           = 0x00;
  h_pcm3168a->dac_control_3           = 0x00;
  h_pcm3168a->adc_sampling_mode       = 0x00;
  h_pcm3168a->adc_control_1           = 0x00;
  h_pcm3168a->adc_control_2           = 0x00;
  h_pcm3168a->adc_input_configuration = 0x00;
  h_pcm3168a->adc_input_phase         = 0x00;
  h_pcm3168a->adc_soft_mute           = 0x00;
  h_pcm3168a->adc_overflow_flag       = 0x00;
  h_pcm3168a->adc_control_3           = 0x00;
  
  for(k = 0; k < 8; k++)
  {
    h_pcm3168a->dac_attenuation[k] = 0xFF;
  }
  
  for(k = 0; k < 6; k++)
  {
    h_pcm3168a->adc_attenuation[k] = 0xD7;
  }
}

// ----------------------------------------------------------------------------
// PCM3168A read device
// ----------------------------------------------------------------------------
void pcm3168a_read(pcm3168a_device* h_pcm3168a)
{
  uint8_t k;
  
  h_pcm3168a->reset_control           = pcm3168a_read_reg(REG_ADDR_RESET_CONTROL);
  h_pcm3168a->dac_control_1           = pcm3168a_read_reg(REG_ADDR_DAC_CONTROL_1);
  h_pcm3168a->dac_control_2           = pcm3168a_read_reg(REG_ADDR_DAC_CONTROL_2);
  h_pcm3168a->dac_output_phase        = pcm3168a_read_reg(REG_ADDR_DAC_OUTPUT_PHASE);
  h_pcm3168a->dac_soft_mute_control   = pcm3168a_read_reg(REG_ADDR_DAC_SOFT_MUTE_CONTROL);
  h_pcm3168a->dac_zero_flag           = pcm3168a_read_reg(REG_ADDR_DAC_ZERO_FLAG);
  h_pcm3168a->dac_control_3           = pcm3168a_read_reg(REG_ADDR_DAC_CONTROL_3);
  h_pcm3168a->adc_sampling_mode       = pcm3168a_read_reg(REG_ADDR_ADC_SAMPLING_MODE);
  h_pcm3168a->adc_control_1           = pcm3168a_read_reg(REG_ADDR_ADC_CONTROL_1);
  h_pcm3168a->adc_control_2           = pcm3168a_read_reg(REG_ADDR_ADC_CONTROL_2);
  h_pcm3168a->adc_input_configuration = pcm3168a_read_reg(REG_ADDR_ADC_INPUT_CONFIGURATION);
  h_pcm3168a->adc_input_phase         = pcm3168a_read_reg(REG_ADDR_ADC_INPUT_PHASE);
  h_pcm3168a->adc_soft_mute           = pcm3168a_read_reg(REG_ADDR_ADC_SOFT_MUTE);
  h_pcm3168a->adc_overflow_flag       = pcm3168a_read_reg(REG_ADDR_ADC_OVERFLOW_FLAG);
  h_pcm3168a->adc_control_3           = pcm3168a_read_reg(REG_ADDR_ADC_CONTROL_3);
  
  for(k = 0; k < 8; k++)
  {
    h_pcm3168a->dac_attenuation[k] = pcm3168a_read_reg(REG_ADDR_DAC_ATTENUATION + k);
  }
  
  for(k = 0; k < 6; k++)
  {
    h_pcm3168a->adc_attenuation[k] = pcm3168a_read_reg(REG_ADDR_ADC_ATTENUATION + k);
  }
}

// ----------------------------------------------------------------------------
// PCM3168A write device
// ----------------------------------------------------------------------------
void pcm3168a_write(pcm3168a_device* h_pcm3168a)
{
  uint8_t k;
  
  pcm3168a_write_reg(REG_ADDR_RESET_CONTROL, h_pcm3168a->reset_control);
  pcm3168a_write_reg(REG_ADDR_DAC_CONTROL_1, h_pcm3168a->dac_control_1);
  pcm3168a_write_reg(REG_ADDR_DAC_CONTROL_2, h_pcm3168a->dac_control_2);
  pcm3168a_write_reg(REG_ADDR_DAC_OUTPUT_PHASE, h_pcm3168a->dac_output_phase);
  pcm3168a_write_reg(REG_ADDR_DAC_SOFT_MUTE_CONTROL, h_pcm3168a->dac_soft_mute_control);
  
  pcm3168a_write_reg(REG_ADDR_DAC_CONTROL_3, h_pcm3168a->dac_control_3);

  pcm3168a_write_reg(REG_ADDR_ADC_SAMPLING_MODE, h_pcm3168a->adc_sampling_mode);
  pcm3168a_write_reg(REG_ADDR_ADC_CONTROL_1, h_pcm3168a->adc_control_1);
  pcm3168a_write_reg(REG_ADDR_ADC_CONTROL_2, h_pcm3168a->adc_control_2);
  pcm3168a_write_reg(REG_ADDR_ADC_INPUT_CONFIGURATION, h_pcm3168a->adc_input_configuration);
  pcm3168a_write_reg(REG_ADDR_ADC_INPUT_PHASE, h_pcm3168a->adc_input_phase);
  pcm3168a_write_reg(REG_ADDR_ADC_SOFT_MUTE, h_pcm3168a->adc_soft_mute);

  pcm3168a_write_reg(REG_ADDR_ADC_CONTROL_3, h_pcm3168a->adc_control_3);
  
  for(k = 0; k < 9; k++)
  {
    pcm3168a_write_reg(REG_ADDR_DAC_ATTENUATION + k, h_pcm3168a->dac_attenuation[k]);
  }

  for(k = 0; k < 7; k++)
  {
    pcm3168a_write_reg(REG_ADDR_ADC_ATTENUATION + k, h_pcm3168a->adc_attenuation[k]);
  } 
}

// ----------------------------------------------------------------------------
// PCM3168A read register
// ----------------------------------------------------------------------------
uint8_t pcm3168a_read_reg(uint8_t addr)
{
  uint8_t buff[2];
  buff[0] = addr | READ_MASK;
  buff[1] = 0x00; // dummy data
  HAL_SPI_Receive(&hspi1, buff, 2, HAL_MAX_DELAY);

  return buff[1];
}

// ----------------------------------------------------------------------------
// PCM3168A write register
// ----------------------------------------------------------------------------
void pcm3168a_write_reg(uint8_t addr, uint8_t data)
{
  uint8_t buff[2];
  buff[1] = data;
  buff[0] = addr;
  HAL_SPI_Transmit(&hspi1, buff, 2, HAL_MAX_DELAY);
}

