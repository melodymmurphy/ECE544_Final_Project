/**
*
* @file sequencer.h
*
* @author Mel Murphy (cumurphy@pdx.edu)
*
* Header for note sequencer functionality
*
******************************************************************************/

#ifndef _SEQUENCER_H
#define _SEQUENCER_H

/****************************** Includes *********************************/
#include "main.h"
#include "signal_generator.h"
#include "midi.h"
#include "luts.h"

/****************************** Definitions ******************************/

// Sequencer values
#define FIRST          	 	0         // first step of sequence
#define LAST           	 	15        // last step of sequence
#define STEPS          	 	16        // number of steps of sequence

#define VELOCITY_MIN		0
#define VELOCITY_MAX		127
#define PITCH_MIN			0
#define PITCH_MAX			127
#define TEMPO_MIN			60		  // slowest tempo
#define TEMPO_MAX			240		  // fastest tempo
#define SWING_MIN			-100
#define SWING_MAX			100
#define RIGHT				0
#define LEFT				1

#define RECORD_BLINK_RATE 	(AXI_CLOCK_FREQ_HZ / 5)		// LED blink rate for RECORD mode
#define BYPASS_BLINK_RATE 	(AXI_CLOCK_FREQ_HZ / 2)

#define TIMER_CLOCK 		AXI_CLOCK_FREQ_HZ


/**************************** Type Definitions ****************************/

// Sequencer modes
typedef enum {PLAY, RECORD, BYPASS, MEMORY} seq_mode_t;
typedef enum {WHOLE, HALF, QUARTER, EIGHTH, SIXTEENTH, THIRTYSECOND, SIXTYFOURTH} note_length_t;
typedef enum {FORWARD, BACKWARD, BOTH_DIR, RANDOM, SUSTAIN} pattern_t;

typedef struct
{
	uint8_t velocity;
	uint8_t pitch;
	uint8_t duty_cycle;
	uint8_t rise_cycle;	// for triangle waves, rise vs. fall percetage

	bool note_on;

} note_t;

typedef struct
{
	uint8_t step;         	 	// sequence step
	uint8_t direction;			// current direction in which sequence is moving (BOTH_DIR implementation)
	uint8_t tempo;				// tempo of sequence in beats per minute
	int8_t swing;				// swing of sequence
	pattern_t pattern;      	// sequence pattern--forwards, backwards, random
	note_length_t subdiv;   	// note subdivisions--whole, half, quarter, etc.
	note_t note[STEPS]; 		// array of note structs assigned to each sequence step
	uint64_t seq_timer;			// timer count for sequence step timer

} sequencer_t;


/************************** Function Prototypes ****************************/

void initializeNote(volatile note_t* note, uint8_t pitch);
void initialize_sequencer(sequencer_t* sequencer);
void states_initialization(sequencer_t* sequencer);
void stepForward(sequencer_t* sequencer);
void stepBackward(sequencer_t* sequencer);
void sequence_step(sequencer_t* sequencer);
uint8_t getStep(sequencer_t* sequencer);
uint32_t tempo_timer(sequencer_t* sequencer, uint8_t tempo);

#endif
