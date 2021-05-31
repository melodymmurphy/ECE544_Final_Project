// Author: Mel Murphy

#ifndef _SEQUENCER_H
#define _SEQUENCER_H

/****************************** Includes *********************************/
#include "main.h"
#include "signal_generator.h"

/****************************** Definitions ******************************/

// Sequencer values
#define FIRST           0         // first step of sequence
#define LAST            15        // last step of sequence
#define STEPS           16        // number of steps of sequence

#define TIMER_CLOCK 	AXI_CLOCK_FREQ_HZ


/**************************** Type Definitions ****************************/

// Sequencer modes
typedef enum { PLAY, RECORD, BYPASS } seq_mode_t;
typedef enum { WHOLE, HALF, QUARTER, EIGHTH, SIXTEENTH, THIRTYSECOND, SIXTYFOURTH } note_length_t;
typedef enum { FORWARD, BACKWARD, BOTH_DIR, RANDOM } pattern_t;

typedef struct
{
  volatile uint8_t step;          	// sequence step
  volatile uint8_t prevStep;		// previous sequencer step
  volatile seq_mode_t mode;         // sequencer mode--PLAY or RECORD (BYPASS functionality will be added with MIDI)
  volatile uint32_t timer_count;   	// value to write to timer countdown
  volatile bool led_toggle;    		// toggle step LED on or off
  volatile pattern_t pattern;      	// sequence pattern--forwards, backwards, random
  volatile note_length_t subdiv;    // note subdivisions--whole, half, quarter, etc.
  volatile note_t note[STEPS]; 		// array of note structs assigned to each sequence step
  uint64_t seq_timer;				// timer count for sequence step timer

} sequencer_t;


/************************** Function Prototypes ****************************/
void initialize_sequencer(sequencer_t* sequencer_p);
void states_initialization(void);
void next_step(void);

#endif
