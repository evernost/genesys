// ============================================================================
// Module name    : ui
// File name      : ui.c
// Purpose        : C library handling Genesys user interface
// Author         : Quentin Biache
// Creation date  : April 2nd, 2022
// ============================================================================

// ============================================================================
// Description
// ============================================================================
// TODO

// ============================================================================
// Includes
// ============================================================================
#include <stdint.h>
#include "ui.h"



// ----------------------------------------------------------------------------
// uiadc_init() function
// ----------------------------------------------------------------------------
void ui_init(*Ui Ui_obj)
{
  
}

// ----------------------------------------------------------------------------
// uiadc_update() function
// ----------------------------------------------------------------------------


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





  
const float midi_note_freq[128] = 
{
  #include "note_scale.txt"
};

const midi_note_event MIDI_NOTE_EVENT_EMPTY = {.index = 0U, .event = 255U, .velocity = 0U};

// ----------------------------------------------------------------------------
// midi_init() function
// ----------------------------------------------------------------------------
// Initialise the MIDI object with default values.
void midi_init(midi* midi_obj)
{
  uint8_t i;
  
  for(i = 0; i < MIDI_MAX_POLY; i++) midi_obj->notes_on[i] = MIDI_CODE_NOTE_OFF;
  midi_obj->note_event_count = 0U;
  midi_obj->interface_status = MIDI_BUFFER_OK;
  for(i = 0; i < 128; i++) midi_obj->ctrl_values[i] = 0;
  midi_obj->pitch_bend = MIDI_PITCH_BEND_DEFAULT;
  
  midi_obj->midi_state = MIDI_STATE_IDLE;
  midi_obj->note_event_ptr_r = 0U;
  midi_obj->note_event_ptr_w = 0U;
}

// ----------------------------------------------------------------------------
// midi_update() function
// ----------------------------------------------------------------------------
// Analyses the incoming MIDI messages and update the internal state variables
// accordingly:
// - store the note events in a FIFO
// - store the controllers values
void midi_update(uint8_t message, midi* midi_obj)
{
  static uint8_t data_count;
  static midi_note_event tmp_event;
  uint8_t ctrl_index;

  switch(midi_obj->midi_state)
  {
    // ------------------------------------------------------------------------
    // MIDI idle state 
    // ------------------------------------------------------------------------
    case MIDI_STATE_IDLE :
      // Init
      data_count = 0;
      tmp_event = MIDI_NOTE_EVENT_EMPTY;
      
      switch(message & 0xF0)
      {
        // Switch depending on the command
        case MIDI_CODE_NOTE_OFF :
          midi_obj->midi_state = MIDI_STATE_NOTE_OFF;
          break;

        case MIDI_CODE_NOTE_ON :
          midi_obj->midi_state = MIDI_STATE_NOTE_ON;
          break;
      
        case MIDI_CODE_CONTROL_CHANGE :
          midi_obj->midi_state = MIDI_STATE_CONTROL_CHANGE;
          break;

        case MIDI_CODE_PROGRAM_CHANGE :
          // No handled yet!
          midi_obj->midi_state = MIDI_STATE_IDLE;
          break;
      
        case MIDI_CODE_PITCH_BEND :
          midi_obj->midi_state = MIDI_STATE_PITCH_BEND;
          break;

        default : 
          // Other commands are not supported
          midi_obj->midi_state = MIDI_STATE_IDLE;   
      }
      break;

    // ------------------------------------------------------------------------
    // MIDI note OFF state 
    // ------------------------------------------------------------------------
    case MIDI_STATE_NOTE_OFF :
      if (data_count == 0U)
      {
        tmp_event.event = MIDI_EVENT_NOTE_OFF;
        tmp_event.index = message;
        data_count++;
      }
      else
      {
        tmp_event.velocity = message;
        midi_note_push(tmp_event, midi_obj);
        data_count = 0U;
        midi_obj->midi_state = MIDI_STATE_IDLE;
      }
      break;

    // ------------------------------------------------------------------------
    // MIDI note ON state 
    // ------------------------------------------------------------------------
    case MIDI_STATE_NOTE_ON :
      if (data_count == 0U)
      {
        tmp_event.event = MIDI_EVENT_NOTE_ON;
        tmp_event.index = message;
        data_count++;
      }
      else
      {
        tmp_event.velocity = message;
        midi_note_push(tmp_event, midi_obj);
        data_count = 0U;
        midi_obj->midi_state = MIDI_STATE_IDLE;
      }
      break;

    // ------------------------------------------------------------------------
    // MIDI control change state 
    // ------------------------------------------------------------------------
    case MIDI_STATE_CONTROL_CHANGE :
      if (data_count == 0U)
      {
        ctrl_index = message;
        data_count++;
      }
      else
      {
        midi_obj->ctrl_values[ctrl_index] = message;
        midi_obj->midi_state = MIDI_STATE_IDLE;
      }
      break;

    // ------------------------------------------------------------------------
    // MIDI program change state 
    // ------------------------------------------------------------------------
    case MIDI_STATE_PROGRAM_CHANGE :
      // No handled yet!
      midi_obj->midi_state = MIDI_STATE_IDLE;
      break;

    // ------------------------------------------------------------------------
    // MIDI pitch bend state 
    // ------------------------------------------------------------------------
    case MIDI_STATE_PITCH_BEND :
      if (data_count == 0U)
      {
        midi_obj->pitch_bend = 0;
        midi_obj->pitch_bend = message << 8;
        data_count++;
      }
      else
      {
        midi_obj->pitch_bend |= message;
        midi_obj->midi_state = MIDI_STATE_IDLE;
      }
      break;

    // ------------------------------------------------------------------------
    // Unsupported MIDI command
    // ------------------------------------------------------------------------
    case MIDI_STATE_UNSUPPORTED :
      // No handled yet!
      midi_obj->midi_state = MIDI_STATE_IDLE;
      break;
  }
}

// ----------------------------------------------------------------------------
// midi_push() function
// ----------------------------------------------------------------------------
void midi_note_push(midi_note_event note, midi* midi_obj)
{
  // Write pointer is not reaching the end of the FIFO
  if ((midi_obj->note_event_count) < MIDI_EVENT_FIFO_SIZE)
  {
    midi_obj->note_event_fifo[midi_obj->note_event_ptr_w] = note;
    midi_obj->note_event_count++;

    // Wrap pointer if reaching the FIFO end
    if ((midi_obj->note_event_ptr_w) == (MIDI_EVENT_FIFO_SIZE-1))
    {
      midi_obj->note_event_ptr_w = 0U;
    }
    else
    {
      midi_obj->note_event_ptr_w++;
    }
    midi_obj->interface_status = MIDI_BUFFER_OK;
  }
  else
  {
    midi_obj->interface_status = MIDI_BUFFER_OVERFLOW;
  }
}

// ----------------------------------------------------------------------------
// midi_pop() function
// ----------------------------------------------------------------------------
midi_note_event midi_note_pop(midi* midi_obj)
{
  if ((midi_obj->note_event_count) > 0)
  {
    midi_note_event note_event;
    note_event = midi_obj->note_event_fifo[midi_obj->note_event_ptr_r];
    midi_obj->note_event_count--;
    
    // Wrap pointer if reaching the FIFO end
    if ((midi_obj->note_event_ptr_r) == (MIDI_EVENT_FIFO_SIZE-1))
    {
      midi_obj->note_event_ptr_r = 0U;
    }
    else
    {
      midi_obj->note_event_ptr_r++;
    }
    midi_obj->interface_status = MIDI_BUFFER_OK;
    return (note_event);
  }
  else
  {
    midi_obj->interface_status = MIDI_BUFFER_EMPTY;
    return MIDI_NOTE_EVENT_EMPTY;
  }
}

