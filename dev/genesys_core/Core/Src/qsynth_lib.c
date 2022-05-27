// ============================================================================
// Module name     : qsynth_lib
// File name       : qsynth_lib.c
// Purpose         : 
// Author          : Quentin Biache
// Creation date   : 04th December, 2021
// ============================================================================

// ============================================================================
// Description
// ============================================================================
// Application entry point.

// ============================================================================
// Includes
// ============================================================================
#include <math.h>
#include <stdint.h>
#include "qsynth_lib.h"

// ----------------------------------------------------------------------------
// Oscillator library
// ----------------------------------------------------------------------------
void osc_step(oscillator* p_osc)
{
  uint32_t a, b;
  float r;
  
  // Increment phase
  p_osc->phase_accu += p_osc->phase_inc;

  // Handle wrap-around
  while((p_osc->phase_accu) >= p_osc->LUT_size)
  {
    p_osc->phase_accu -= p_osc->LUT_size;
  }

  while((p_osc->phase_accu) < 0.0f)
  {
    p_osc->phase_accu += p_osc->LUT_size;
  }

  // Interpolate to reduce spurious dramatically
  a = ((uint32_t)(p_osc->phase_accu)) % (p_osc->LUT_size);
  b = (a + 1) % (p_osc->LUT_size);
  r = p_osc->phase_accu - ((uint32_t)p_osc->phase_accu);
  
  // Interpolate
  p_osc->output = (1.0f - r)*(*(p_osc->p_LUT_osc + a)) + r*(*(p_osc->p_LUT_osc + b));
}

void osc_reset_phase(oscillator* p_osc)
{
  p_osc->phase_accu = 0.0f;
  p_osc->output = *(p_osc->p_LUT_osc);
}

// ----------------------------------------------------------------------------
// ADSR library
// ----------------------------------------------------------------------------
void adsr_init(adsr* adsr_obj)
{
  adsr_obj -> state       = ADSR_STATE_IDLE;
  adsr_obj -> ratio       = 0.0f;
  adsr_obj -> busy        = ADSR_DONE;
  adsr_obj -> output      = 0.0f;
  adsr_obj -> note_status = KEY_RELEASED;
  adsr_obj -> note_event  = 0;
  adsr_obj -> prescaler   = 1;
}

void adsr_step(adsr* adsr_obj)
{
  switch(adsr_obj -> state)
  {
    // ------------------------------------------------------------------------
    // ADSR idle state 
    // ------------------------------------------------------------------------
    case ADSR_STATE_IDLE :
    
      // Key pressed
      if (((adsr_obj -> note_event) == 1) && ((adsr_obj -> note_status) == KEY_PRESSED))
      {
        // Reset the event to indicate that it has been taken into account
        adsr_obj -> note_event = 0;
        
        // Skip the attack phase if the attack time is too short
        if ((adsr_obj -> attack_time) < ADSR_MIN_ATTACK_TIME)
        {
          // Skip the decay phase if the decay time is too short
          if ((adsr_obj -> decay_time) < ADSR_MIN_DECAY_TIME)
          {
            // Skip the sustain + release if sustain is too low
            if ((adsr_obj -> sustain_level) < ADSR_MIN_SUSTAIN_LEVEL)
            {
              adsr_obj -> output  = 0.0f;
              adsr_obj -> busy    = ADSR_DONE;
              adsr_obj -> state   = ADSR_STATE_IDLE;
            }
            else
            {
              adsr_obj -> output  = adsr_obj -> sustain_level;
              adsr_obj -> busy    = ADSR_BUSY;
              adsr_obj -> state   = ADSR_STATE_SUSTAIN;
            }
          }
          // Skip attack only
          else
          {
            adsr_obj -> ratio   = adsr_time_to_ratio(adsr_obj -> decay_time);
            adsr_obj -> output  = 1.0f;
            adsr_obj -> busy    = ADSR_BUSY;
            adsr_obj -> state   = ADSR_STATE_DECAY;
          }
        }
        // Nominal attack
        else
        {
          adsr_obj -> ratio   = adsr_time_to_ratio(adsr_obj -> attack_time);
          adsr_obj -> output  = (adsr_obj -> ratio) * (adsr_obj -> output - 1.0f) + 1.0f;
          adsr_obj -> busy    = ADSR_BUSY;
          adsr_obj -> state   = ADSR_STATE_ATTACK;
        }
      }
      
      // No keypress
      else
      {
        // Nothing to do.
      }

      break;

    // ------------------------------------------------------------------------
    // ADSR attack state 
    // ------------------------------------------------------------------------
    case ADSR_STATE_ATTACK :
    
      // Key released
      if (((adsr_obj -> note_event) == 1) && ((adsr_obj -> note_status) == KEY_RELEASED))
      {
        // Reset the event
        adsr_obj -> note_event = 0;
      
        // Release time too short
        if ((adsr_obj -> release_time) < ADSR_MIN_RELEASE_TIME)
        {
          adsr_obj -> output  = 0.0f;
          adsr_obj -> busy    = ADSR_DONE;
          adsr_obj -> state   = ADSR_STATE_IDLE;
        }
        // Nominal release
        else
        {
          adsr_obj -> ratio   = adsr_time_to_ratio(adsr_obj -> release_time);
          adsr_obj -> output  = (adsr_obj -> ratio) * (adsr_obj -> output);
          adsr_obj -> state   = ADSR_STATE_RELEASE;
        }
      }

      // Key pressed (impossible?)
      else if (((adsr_obj -> note_event) == 1) && ((adsr_obj -> note_status) == KEY_PRESSED))
      {
        // Reset the event
        adsr_obj -> note_event = 0;
      }
      
      // No key event
      else
      {
        // Detect the end of the attack curve
        if ((adsr_obj -> output) >= ADSR_RISE_DONE_THRESH)
        {
          adsr_obj -> ratio         = adsr_time_to_ratio(adsr_obj -> decay_time);
          adsr_obj -> output        = (adsr_obj -> ratio) * (adsr_obj -> output);
          adsr_obj -> skip_sustain  = (adsr_obj -> sustain_level < ADSR_MIN_SUSTAIN_LEVEL) ? 1U : 0U;
          adsr_obj -> state         = ADSR_STATE_DECAY;
        }
        else
        {
          adsr_obj -> output  = (adsr_obj -> ratio) * (adsr_obj -> output - 1.0f) + 1.0f;
          adsr_obj -> state   = ADSR_STATE_ATTACK;
        }
      }
      
      break;

    // ------------------------------------------------------------------------
    // ADSR decay state 
    // ------------------------------------------------------------------------
    case ADSR_STATE_DECAY :
      
      // Key released
      if (((adsr_obj -> note_event) == 1) && ((adsr_obj -> note_status) == KEY_RELEASED))
      {
        // Reset the event
        adsr_obj -> note_event = 0;

        // Release time too short
        if ((adsr_obj -> release_time) < ADSR_MIN_RELEASE_TIME)
        {
          adsr_obj -> output  = 0.0f;
          adsr_obj -> busy    = ADSR_DONE;
          adsr_obj -> state   = ADSR_STATE_IDLE;
        }
        // Nominal release
        else
        {
          adsr_obj -> ratio   = adsr_time_to_ratio(adsr_obj -> release_time);
          adsr_obj -> output  = (adsr_obj -> ratio) * (adsr_obj -> output);
          adsr_obj -> state   = ADSR_STATE_RELEASE;
        }
      }
      
      // Key pressed (impossible?)
      else if (((adsr_obj -> note_event) == 1) && ((adsr_obj -> note_status) == KEY_PRESSED))
      {
        // Reset the event
        adsr_obj -> note_event = 0U;
      }
      
      // No key event: nominal decay until sustain
      else
      {
        // Sustain level is too low. Don't wait to reach it
        if (((adsr_obj -> skip_sustain) == 1U) && ((adsr_obj -> output) <= ADSR_MIN_SUSTAIN_LEVEL))
        {
          adsr_obj -> output  = 0.0f;
          adsr_obj -> busy    = ADSR_DONE;
          adsr_obj -> state   = ADSR_STATE_IDLE;
        }
        // Sustain level is hit
        else if ((adsr_obj -> output) < (adsr_obj -> sustain_level))
        {
          adsr_obj -> output  = adsr_obj -> sustain_level;
          adsr_obj -> state   = ADSR_STATE_SUSTAIN;
        }
        // Nominal decay
        else
        {
          adsr_obj -> output = (adsr_obj -> ratio) * (adsr_obj -> output);
        }
      }

      break;

    // ------------------------------------------------------------------------
    // ADSR sustain state 
    // ------------------------------------------------------------------------
    case ADSR_STATE_SUSTAIN :
      
      // Key released
      if (((adsr_obj -> note_event) == 1) && ((adsr_obj -> note_status) == KEY_RELEASED))
      {
        // Reset the event
        adsr_obj -> note_event = 0;

        // Release time too short
        if ((adsr_obj -> release_time) < ADSR_MIN_RELEASE_TIME)
        {
          adsr_obj -> output  = 0.0f;
          adsr_obj -> busy    = ADSR_DONE;
          adsr_obj -> state   = ADSR_STATE_IDLE;
        }
        // Nominal release
        else
        {
          adsr_obj -> ratio   = adsr_time_to_ratio(adsr_obj -> release_time);
          adsr_obj -> output  = (adsr_obj -> ratio) * (adsr_obj -> output);
          adsr_obj -> state   = ADSR_STATE_RELEASE;
        }
      }
      
      // Key pressed (impossible?)
      else if (((adsr_obj -> note_event) == 1) && ((adsr_obj -> note_status) == KEY_PRESSED))
      {
        // Reset the event
        adsr_obj -> note_event = 0;
      }
      
      // No key event
      else
      {
        // Nothing to do.
      }

      break;

    // ------------------------------------------------------------------------
    // ADSR release state 
    // ------------------------------------------------------------------------
    case ADSR_STATE_RELEASE :
      
      // Key released
      if (((adsr_obj -> note_event) == 1) && ((adsr_obj -> note_status) == KEY_RELEASED))
      {
        // Reset the event
        adsr_obj -> note_event = 0;

        // Nothing to do.
        // FSM is in release state because the key has been released already.
      }
      
      // Key pressed
      else if (((adsr_obj -> note_event) == 1) && ((adsr_obj -> note_status) == KEY_PRESSED))
      {
        // Reset the event
        adsr_obj -> note_event = 0;

        // Ignore.
        // If a key is pressed but the signal is not fully released yet, it has to be 
        // assigned to another voice and the corresponding ADSR must be triggered.
      }
      
      // No key event
      else
      {
        if (adsr_obj -> output < ADSR_RELEASE_DONE_THRESH)
        {
          adsr_obj -> output  = 0.0f;
          adsr_obj -> busy    = ADSR_DONE;
          adsr_obj -> state   = ADSR_STATE_IDLE;
        }
        else
        {
          adsr_obj -> output = (adsr_obj -> ratio) * (adsr_obj -> output);
        }
      }

      break;
  }
}

// t = normalized time in 0...1
// When t = 1.0, actual time is about XXX
float adsr_time_to_ratio(float t)
{
  // Designed in Geogebra, then tweaked in matlab script
  const float a_0 = 7.533093301515650f;
  const float a_1 = -6.666185180470015f;
  const float a_2 = -0.648421215312096f;
  const float a_3 = -0.152300302702238f;
  
  const float p_1 = 0.05f;
  const float p_2 = 0.141979205962700f;
  const float p_3 = 0.270386037823100f;
  
  const float C   = 0.466551463709798f;

  if (t >= 0.0f)
  {
    return (a_0*fabs(t) + a_1*fabs(t-p_1) + a_2*fabs(t-p_2) + a_3*fabs(t-p_3) + C);
  }
  else if (t >= 1.0f)
  {
    return 1.0f;
  }
  else
  {
    return 0.0f;
  }
}
