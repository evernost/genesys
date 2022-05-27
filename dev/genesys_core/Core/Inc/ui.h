// ============================================================================
// Module name    : ui
// File name      : ui.h
// Purpose        : C library handling Genesys user interface
// Author         : Quentin Biache
// Creation date  : April 2nd, 2022
// ============================================================================

// Define to prevent recursive inclusion --------------------------------------
#ifndef __UI_H
#define __UI_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes -------------------------------------------------------------------


// Defines --------------------------------------------------------------------
#define UI_EVENT_KEY_RELEASE  0U
#define UI_EVENT_KEY_PRESS    1U
#define UI_EVENT_KNOB_CHANGE  2U

#define UI_ID_KNOB_A      0U
#define UI_ID_KNOB_D      1U
#define UI_ID_KNOB_S      2U
#define UI_ID_KNOB_R      3U
#define UI_ID_KNOB_LU     4U
#define UI_ID_KNOB_LD     5U
#define UI_ID_JOYSTICK_X  6U
#define UI_ID_JOYSTICK_Y  7U
#define UI_ID_PB_U        8U
#define UI_ID_PB_D        9U
#define UI_ID_PB_L        10U
#define UI_ID_PB_R        11U
#define UI_ID_PB_CTR      12U
#define UI_ID_PB_A        13U
#define UI_ID_PB_B        14U

// Types ----------------------------------------------------------------------

typedef enum
{
  UI_ACQ_STATE_IDLE   = 0U,
  UI_ACQ_STATE_COMMIT = 1U,
} ui_acq_state_type;


struct Ui
{
  float knob_a;
  float knob_d;
  float knob_s;
  float knob_r;
  float knob_lu;
  float knob_ld;
  float joystick_x;
  float joystick_y;
  uint8_t pb_u;
  uint8_t pb_d;
  uint8_t pb_l;
  uint8_t pb_r;
  uint8_t pb_ctr;
  uint8_t pb_a;
  uint8_t pb_b;
  uint8_t N_avg_knob;
  uint8_t avg_cnt;      // Averaging counter (private)
  uint16_t event_cnt;   // Number of unread ui events 
  ui_state_type state;  // Acquisition FSM state (private)
};
typedef struct Ui Ui;

struct Ui_event
{
  uint8_t knob_id;
  uint8_t event;
};
typedef struct Ui_event Ui_event;


// Functions prototypes -------------------------------------------------------

// Public
void ui_init(Ui*);
Ui_event ui_event_pop(Ui*);


#ifdef __cplusplus
}
#endif

#endif // __UI_H
