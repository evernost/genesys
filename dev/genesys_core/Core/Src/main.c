// ============================================================================
// Module name     : main 
// File name       : main.c
// Purpose         : genesys entry point
// Author          : Quentin Biache
// Creation date   : 08th August, 2021
// ============================================================================

// ============================================================================
// Description
// ============================================================================
// Application entry point.

// ============================================================================
// Includes
// ============================================================================
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "ltdc.h"
#include "quadspi.h"
#include "sai.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "pcm3168a.h"
#include "qsynth_lib.h"
#include "midi.h"

// Private includes -----------------------------------------------------------
#include <string.h>
#include <stdio.h>

// Private typedef ------------------------------------------------------------


// Private define -------------------------------------------------------------


// Private macro --------------------------------------------------------------


// Private variables ----------------------------------------------------------


// Private function prototypes ------------------------------------------------
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);

// Private user code ----------------------------------------------------------

// ----------------------------------------------------------------------------
// UART console management 
// ----------------------------------------------------------------------------
volatile uint8_t UART_unread = 0;
uint8_t UART_rx_buff[10];

// ----------------------------------------------------------------------------
// MIDI management
// ----------------------------------------------------------------------------
volatile uint8_t MIDI_unread = 0;
uint8_t MIDI_rx_buff[10];

// ----------------------------------------------------------------------------
// Audio interface
// ----------------------------------------------------------------------------
volatile uint8_t SAI_transmit_done = 0;
volatile uint8_t available_buff = 0;

// Audio samples buffers size (in bytes)
// 512 = 2 channels x 4 bytes per sample x 64 samples.
#define SAMPLES_BUFF_SIZE_WORDS 128U
#define SAMPLES_BUFF_SIZE_BYTES (4U*SAMPLES_BUFF_SIZE_WORDS)
#define SAMPLE_RATE 48000.0f

uint8_t samples_buff_TX[SAMPLES_BUFF_SIZE_BYTES];
uint8_t samples_buff_RX[SAMPLES_BUFF_SIZE_BYTES];

// ----------------------------------------------------------------------------
// UI ADC management
// ----------------------------------------------------------------------------
uint32_t adc_val;
uint8_t ADC_msg_unread;
uint32_t ADC_channel = 0;

// ----------------------------------------------------------------------------
// Callbacks
// ----------------------------------------------------------------------------

// Tx Transfer Half completed callback
// First half of the buffer has been sent.
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  SAI_transmit_done = 1;
  available_buff    = 0;
}

// Tx Transfer completed callback
// Second half of the buffer has been sent.
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{  
  // First, start the new transfer
  HAL_SAI_Transmit_DMA(&hsai_BlockA1, samples_buff_TX, SAMPLES_BUFF_SIZE_WORDS);

  // Second, update the flags
  SAI_transmit_done = 1;
  available_buff    = 1;
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
  __NOP();
}

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  __NOP();
}

// ----------------------------------------------------------------------------
// Wave LUT
// ----------------------------------------------------------------------------
#define WAVE_LUT_SIZE 256U
float wave_LUT[WAVE_LUT_SIZE] = 
{
  #include "wave_LUT.txt"
};

// ----------------------------------------------------------------------------
// main()
// @brief  The application entry point.
// @retval int
// ----------------------------------------------------------------------------
int main(void)
{
  // move me in a driver plz
  uint8_t pcm3168a_enable = 0;
  uint8_t DAC_send_enable = 0;
  int8_t atten_dB = -20;
  uint8_t atten_reg = (uint8_t)((atten_dB << 1) + 255U);

  float f_0 = 110.0f;
  float f_1 = 440.5f;
  float r_m = 0.1f;
  float a = 1.0f;
  
  // Objects
  oscillator osc_0, osc_1, osc_2, osc_3, osc_4;
  adsr adsr_vca, adsr_fm;
  midi midi_obj;

  midi_note_event note;
  
  // MCU Configuration---------------------------------------------------------

  // Reset of all peripherals, Initializes the Flash interface and the Systick.
  HAL_Init();

  // Configure the system clock
  SystemClock_Config();

  // Configure the peripherals common clocks
  PeriphCommonClock_Config();

  // Initialize all configured peripherals
  MX_DMA_Init();    // <----- MX_DMA_Init() MUST be done first. Otherwise DMA function is jeopardized
  MX_GPIO_Init();
  MX_ADC3_Init();
  MX_LTDC_Init();
  MX_QUADSPI_Init();
  MX_SAI1_Init();
  MX_SPI1_Init();
  MX_SPI4_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_USB_OTG_FS_HCD_Init();
  MX_TIM2_Init();
  
  // Start ADC
  if (HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
  {
    console("ADC calibration failed\r\n");
  }
  else
  {
    console("ADC calibration OK\r\n");
  }

  HAL_ADC_Start_IT(&hadc3);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

  HAL_UART_Receive_IT(&huart4, UART_rx_buff, 1);
  HAL_UART_Receive_IT(&huart5, MIDI_rx_buff, 1);
  console("Init done!\r\n");
  console("Ready.\r\n"); 

  // Initialise oscillators
  osc_0.p_LUT_osc   = wave_LUT;
  osc_0.LUT_size    = WAVE_LUT_SIZE;
  osc_0.phase_accu  = 0.0f;
  osc_0.phase_inc   = WAVE_LUT_SIZE*f_0/SAMPLE_RATE;
  osc_0.output      = osc_0.p_LUT_osc[0];

  osc_1.p_LUT_osc   = wave_LUT;
  osc_1.LUT_size    = WAVE_LUT_SIZE;
  osc_1.phase_accu  = 0.0f;
  osc_1.phase_inc   = WAVE_LUT_SIZE*f_0/SAMPLE_RATE;
  osc_1.output      = osc_1.p_LUT_osc[0];

  osc_2.p_LUT_osc   = wave_LUT;
  osc_2.LUT_size    = WAVE_LUT_SIZE;
  osc_2.phase_accu  = 0.0f;
  osc_2.phase_inc   = WAVE_LUT_SIZE*f_0/SAMPLE_RATE;
  osc_2.output      = osc_2.p_LUT_osc[0];

  osc_3.p_LUT_osc   = wave_LUT;
  osc_3.LUT_size    = WAVE_LUT_SIZE;
  osc_3.phase_accu  = 0.0f;
  osc_3.phase_inc   = WAVE_LUT_SIZE*f_0/SAMPLE_RATE;
  osc_3.output      = osc_3.p_LUT_osc[0];

  adsr_init(&adsr_vca);
  adsr_init(&adsr_fm);
  midi_init(&midi_obj);

  // --------------------------------------------------------------------------
  // Main loop
  // --------------------------------------------------------------------------
  while(1)
  {
    
    // ADC handling
    if (ADC_msg_unread == 1U)
    {
      char msg[64];
      ADC_msg_unread = 0U;

      sprintf(msg, "Channel #%d: 0x%08X\r\n", ADC_channel,adc_val);
      HAL_UART_Transmit(&huart4, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

      ADC_channel = (ADC_channel+1) & 0x7;
      GPIOD->ODR = (GPIOD->IDR & 0xFFFFC7FF) | (ADC_channel << 11U);
    }

    // Handle MIDI messages, update FSM
    if (MIDI_unread == 1U) 
    {
      MIDI_unread = 0U;
      midi_update(MIDI_rx_buff[0], &midi_obj);
    }

    // Pop MIDI events if any
    // TODO: replace with a while()
    if (midi_obj.note_event_count > 0) 
    {
      note = midi_note_pop(&midi_obj);
      if (note.event == MIDI_EVENT_NOTE_ON) 
      {
        //TODO: start ADSR here
        a = 1.0f;
        r_m = 7.0f;
        f_0 = midi_note_freq[note.index];
        f_1 = 3.02f*f_0;
      }
    }

    // UART interaction FSM
    if (UART_unread == 1)
    {
      UART_unread = 0;

      // ----------------------------------------------------------------------
      // Turn on the ADC/DAC
      // ----------------------------------------------------------------------
      if (UART_rx_buff[0] == 'o')
      {
        if (pcm3168a_enable == 0)
        {
          HAL_GPIO_WritePin(user_LED_left_GPIO_Port, user_LED_left_Pin, GPIO_PIN_SET);
          HAL_GPIO_WritePin(audio_codec_reset_GPIO_Port, audio_codec_reset_Pin, GPIO_PIN_SET);
          pcm3168a_enable = 1;

          console("PCM3168A is turned ON.\r\n");
        }
        else
        {
          HAL_GPIO_WritePin(user_LED_left_GPIO_Port, user_LED_left_Pin, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(audio_codec_reset_GPIO_Port, audio_codec_reset_Pin, GPIO_PIN_RESET);
          pcm3168a_enable = 0;

          console("PCM3168A is turned OFF.\r\n");
        }
      }

      // ----------------------------------------------------------------------
      // Send SPI read all command
      // ----------------------------------------------------------------------
      if (UART_rx_buff[0] == 'r')
      {
        uint8_t addr;
        char msg[64];

        console("ADC/DAC current regmap:\r\n");
        for(addr = 0x40; addr <= 0x5E; addr++)
        { 
          sprintf(msg, "0x%02X: 0x%02X\r\n", addr, pcm3168a_read_reg(addr));
          HAL_UART_Transmit(&huart4, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        }
        console("Done.\r\n");
      }

      // ----------------------------------------------------------------------
      // Send SPI write command
      // ----------------------------------------------------------------------
      if (UART_rx_buff[0] == 'w')
      {
        uint8_t channel;

        pcm3168a_write_reg(REG_ADDR_DAC_CONTROL_1, 0x87);
        pcm3168a_write_reg(REG_ADDR_ADC_CONTROL_1, 0x07);

        for(channel = 0; channel < 9; channel++) pcm3168a_write_reg(REG_ADDR_DAC_ATTENUATION + channel, atten_reg);
        for(channel = 0; channel < 7; channel++) pcm3168a_write_reg(REG_ADDR_ADC_ATTENUATION + channel, 0xC3);
        console("Done.\r\n");
      }

      // ----------------------------------------------------------------------
      // Send a chunk of data
      // ----------------------------------------------------------------------
      if (UART_rx_buff[0] == 's')
      {
        if (DAC_send_enable == 0)
        {
          DAC_send_enable = 1;
          console("DMA data stream is ON.\r\n");
          HAL_SAI_Transmit_DMA(&hsai_BlockA1, samples_buff_TX, SAMPLES_BUFF_SIZE_WORDS);
        }
        else
        {
          DAC_send_enable = 0;
          console("DMA data stream is OFF.\r\n");
        }
      }

      if (UART_rx_buff[0] == 'a')
      {
        HAL_SAI_Receive_DMA(&hsai_BlockB1, samples_buff_RX, SAMPLES_BUFF_SIZE_WORDS);
        console("Got that babe.\r\n");
      }

      // ----------------------------------------------------------------------
      // Manual trig
      // ----------------------------------------------------------------------
      if (UART_rx_buff[0] == 't')
      {
        a = 1.0f;
        r_m = 8.0f;
      }

      // ----------------------------------------------------------------------
      // Adjust volume
      // ----------------------------------------------------------------------
      if ((UART_rx_buff[0] == 'p') || (UART_rx_buff[0] == 'm'))
      {
        uint8_t channel;
        uint8_t real_att;
        char tmp[64];

        // Handle bounds
        if ((UART_rx_buff[0] == 'p') && (atten_reg < 255))
          atten_reg++;

        if ((UART_rx_buff[0] == 'm') && (atten_reg > 0))
          atten_reg--;
        
        // Commit changes        
        for(channel = 0; channel < 9; channel++) pcm3168a_write_reg(REG_ADDR_DAC_ATTENUATION + channel, atten_reg);
      }
    }

    // ------------------------------------------------------------------------
    // Audio stream management
    // ------------------------------------------------------------------------
    if ((DAC_send_enable == 1) && (SAI_transmit_done == 1))
    {
      uint16_t k = 0;
      int32_t output;
      
      SAI_transmit_done = 0;
      
      // Visual feedback: start buffering
      //HAL_GPIO_WritePin(user_LED_right_GPIO_Port, user_LED_right_Pin, GPIO_PIN_SET);
      
      // Fill in the next buffer
      for(k = 0; k < (SAMPLES_BUFF_SIZE_BYTES/2); k += 8)
      {
        output = (int32_t)(a*(0.2f*osc_0.output + 0.2f*osc_2.output + 0.2f*osc_3.output)*(8388607.0f));

        // Fill buffer
        if (available_buff == 0)
        {
          *((int32_t*)(samples_buff_TX + k + 0)) = output; // left channel
          *((int32_t*)(samples_buff_TX + k + 4)) = output; // right channel
        }
        else
        {
          *((int32_t*)(samples_buff_TX + k + 0 + (SAMPLES_BUFF_SIZE_BYTES/2))) = output;
          *((int32_t*)(samples_buff_TX + k + 4 + (SAMPLES_BUFF_SIZE_BYTES/2))) = output;
        } 

        // Progress in the oscillator curve
        // m(k) = f_0*(1 + r_m*osc_1.output);

        // % Update  
        // osc_0.step(LUT_size*m(k)/f_s);
        // osc_1.step(LUT_size*f_m/f_s);

        // OSC1 => OSC0
        osc_1.phase_inc = f_1*(0.0053333333333f);
        osc_0.phase_inc = f_0*(1.0f + (r_m*osc_1.output))*(0.0053333333333f);
        
        osc_2.phase_inc = 0.5f*f_0*(0.0053333333333f);
        osc_3.phase_inc = 2.02f*f_0*(0.0053333333333f);

        osc_step(&osc_0);
        osc_step(&osc_1);
        osc_step(&osc_2);
        osc_step(&osc_3);

        adsr_step(&adsr_vca);
        adsr_step(&adsr_fm);
        r_m *= 0.9999f;
        a *= 0.9999f;
      }
      
      // Visual feedback: buffering done
      //HAL_GPIO_WritePin(user_LED_right_GPIO_Port, user_LED_right_Pin, GPIO_PIN_RESET);
    }

  }
}

// ----------------------------------------------------------------------------
// @brief System Clock Configuration
// @retval None
// ----------------------------------------------------------------------------
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState        = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM        = 5;
  RCC_OscInitStruct.PLL.PLLN        = 192;
  RCC_OscInitStruct.PLL.PLLP        = 2;
  RCC_OscInitStruct.PLL.PLLQ        = 2;
  RCC_OscInitStruct.PLL.PLLR        = 2;
  RCC_OscInitStruct.PLL.PLLRGE      = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL   = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN    = 0;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider   = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider   = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider  = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider  = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider  = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider  = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_ADC
                              |RCC_PERIPHCLK_SAI1|RCC_PERIPHCLK_SPI1
                              |RCC_PERIPHCLK_SPI4|RCC_PERIPHCLK_UART5
                              |RCC_PERIPHCLK_UART4|RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLL2.PLL2M = 12;
  PeriphClkInitStruct.PLL2.PLL2N = 118;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 128;
  PeriphClkInitStruct.PLL2.PLL2R = 128;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_1;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.PLL3.PLL3M = 25;
  PeriphClkInitStruct.PLL3.PLL3N = 192;
  PeriphClkInitStruct.PLL3.PLL3P = 4;
  PeriphClkInitStruct.PLL3.PLL3Q = 4;
  PeriphClkInitStruct.PLL3.PLL3R = 4;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL2;
  PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL3;
  PeriphClkInitStruct.Spi45ClockSelection = RCC_SPI45CLKSOURCE_PLL3;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_PLL3;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  // USER CODE BEGIN Error_Handler_Debug
  // User can add his own implementation to report the HAL error return state.
  __disable_irq();
  while (1)
  {
    console("Shit happened.\r\n");
  }
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
  // USER CODE BEGIN
  // User can add his own implementation to report the file name and line number,
  // ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line)
}
#endif

// ----------------------------------------------------------------------------
// Function HAL_UART_RxCpltCallback()
// @brief  TODO
// @retval None.
// ----------------------------------------------------------------------------
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart4)
  {
    UART_unread = 1U;
    HAL_UART_Receive_IT(&huart4, UART_rx_buff, 1);  // dont ask me why it has to be done here.
  }
  else if (huart == &huart5)
  {
    MIDI_unread = 1U;
    HAL_UART_Receive_IT(&huart5, MIDI_rx_buff, 1);
  }

}

// ----------------------------------------------------------------------------
// Function console()
// @brief  TODO
// @retval None.
// ----------------------------------------------------------------------------
void console(const char *msg)
{
  char tmp[64];
  sprintf(tmp, msg);
  HAL_UART_Transmit(&huart4, (uint8_t *)tmp, strlen(tmp), HAL_MAX_DELAY);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim2)
  {
    HAL_GPIO_TogglePin(user_LED_right_GPIO_Port, user_LED_right_Pin);
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  adc_val = HAL_ADC_GetValue(&hadc3);
  ADC_msg_unread = 1U;
}


