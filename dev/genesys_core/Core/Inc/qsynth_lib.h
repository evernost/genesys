// ============================================================================
// Module name     : qsynth_lib 
// File name       : qsynth_lib.h
// Purpose         : 
// Author          : Quentin Biache
// Creation date   : 04th December, 2021
// ============================================================================

// Define to prevent recursive inclusion --------------------------------------
#ifndef __QSYNTH_LIB_H
#define __QSYNTH_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes -------------------------------------------------------------------


// Defines --------------------------------------------------------------------
#define ADSR_MIN_ATTACK_TIME    0.005f
#define ADSR_MIN_DECAY_TIME     0.005f
#define ADSR_MIN_SUSTAIN_LEVEL  0.005f  // Below this, don't bother to "release" and just discard it.
#define ADSR_MIN_RELEASE_TIME   0.005f

#define ADSR_RISE_DONE_THRESH   0.99f
#define ADSR_FALL_DONE_THRESH   0.01f

#define ADSR_RELEASE_DONE_THRESH  0.001f  // <ADSR_FALL_DONE_THRESH> value might be too high

#define KEY_PRESSED   1U
#define KEY_RELEASED  0U

#define ADSR_BUSY 1U
#define ADSR_DONE 0U

// Types ----------------------------------------------------------------------

// Oscillator type
struct oscillator
{
  float     *p_LUT_osc; // pointer to the LUT containing the oscillator wave
  uint32_t  LUT_size;   // size of the LUT
  float     phase_accu; // private
  float     phase_inc;
  float     output;     // oscillator's output
};
typedef struct oscillator oscillator;

// ADSR object type
typedef enum
{
  ADSR_STATE_IDLE     = 0U,
  ADSR_STATE_ATTACK   = 1U,
  ADSR_STATE_DECAY    = 2U,
  ADSR_STATE_SUSTAIN  = 3U,
  ADSR_STATE_RELEASE  = 4U
} adsr_state;

struct adsr
{
  uint32_t    prescaler;      // number of steps requests for an actual step
  float       attack_time;    // attack time (between 0.0 and 1.0)
  float       decay_time;     // decay time (between 0.0 and 1.0)
  float       sustain_level;  // sustain level (between 0.0 and 1.0)
  float       release_time;   // release time (between 0.0 and 1.0)
  uint8_t     note_status;    // input note status (0 = note off or released, 1 = key pressed)
  uint8_t     note_event;     // set by user when key event, reset by software
  uint8_t     skip_sustain;   // private (internal)
  float       ratio;          // private (internal)
  adsr_state  state;          // private (internal)
  uint8_t     busy;           // goes back to 0 when ADSR state is done
  float       output;         // ADSR's output
};
typedef struct adsr adsr;


// Functions prototypes -------------------------------------------------------

// Oscillator
void osc_step(oscillator*);

// ADSR
void adsr_init(adsr*);
void adsr_step(adsr*);
float adsr_time_to_ratio(float);

#ifdef __cplusplus
}
#endif

#endif // __QSYNTH_LIB_H
