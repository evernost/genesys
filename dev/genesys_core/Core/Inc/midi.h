// ============================================================================
// Module name     : midi 
// File name       : midi.h
// Purpose         : TI PCM3168A ADC/DAC low level driver.
// Author          : Tian Xia, Quentin Biache
// Creation date   : 17th October, 2021
// ============================================================================

// Define to prevent recursive inclusion --------------------------------------
#ifndef __MIDI_H
#define __MIDI_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes -------------------------------------------------------------------


// Defines --------------------------------------------------------------------
#define MIDI_CODE_NOTE_OFF          0x80
#define MIDI_CODE_NOTE_ON           0x90
#define MIDI_CODE_POLY_KEY_PRESSURE 0xA0
#define MIDI_CODE_CONTROL_CHANGE    0xB0
#define MIDI_CODE_PROGRAM_CHANGE    0xC0
#define MIDI_CODE_CHANNEL_PRESSURE  0xD0
#define MIDI_CODE_PITCH_BEND        0xE0

#define MIDI_MAX_POLY         10U

#define MIDI_EVENT_FIFO_SIZE  20U
#define MIDI_EVENT_NOTE_OFF   0U
#define MIDI_EVENT_NOTE_ON    1U

// Default value TBC
#define MIDI_PITCH_BEND_DEFAULT 127U

// Types ----------------------------------------------------------------------
typedef enum
{
  MIDI_STATE_IDLE           = 0U,
  MIDI_STATE_NOTE_ON        = 1U,
  MIDI_STATE_NOTE_OFF       = 2U,
  MIDI_STATE_CONTROL_CHANGE = 3U,
  MIDI_STATE_PROGRAM_CHANGE = 4U,
  MIDI_STATE_PITCH_BEND     = 5U,
  MIDI_STATE_UNSUPPORTED    = 6U
} midi_state_type;

typedef enum 
{
  MIDI_BUFFER_OK        = 0U,
  MIDI_BUFFER_OVERFLOW  = 1U,
  MIDI_BUFFER_EMPTY     = 2U
} midi_status_type;

struct midi_note_event
{
  uint8_t index;
  uint8_t event;
  uint8_t velocity;
};
typedef struct midi_note_event midi_note_event;

struct midi
{
  // Public
  uint8_t notes_on[MIDI_MAX_POLY];  // array containing all the current notes ON
  uint8_t note_event_count;         // number of note events in the FIFO
  uint8_t interface_status;         // indicate various status like note buffer overflow
  uint8_t ctrl_values[128];         // snapshot of the controllers values
  uint16_t pitch_bend;              // current pitch bend
  
  // Private
  midi_state_type midi_state;
  midi_status_type midi_status;
  midi_note_event note_event_fifo[MIDI_EVENT_FIFO_SIZE];
  uint8_t note_event_ptr_r;   // Note event FIFO read pointer
  uint8_t note_event_ptr_w;   // Note event FIFO write pointer
};
typedef struct midi midi;

// These constants have to be seen when "midi.h" is included.
// The "extern" indicates that these are defined elsewhere (in this case, in midi.c)
// Definition cannot be given here, otherwise these constants would be defined
// everytime "midi.h" is included.
extern const float midi_note_freq[128];
extern const midi_note_event MIDI_NOTE_EVENT_EMPTY;

// Functions prototypes -------------------------------------------------------
// Public
void midi_init(midi*);
midi_note_event midi_note_pop(midi*);
void midi_update(uint8_t, midi*);

// Private
void midi_note_push(midi_note_event, midi*);

#ifdef __cplusplus
}
#endif

#endif // __MIDI_H
