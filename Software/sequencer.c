// Author: Mel Murphy

#include "sequencer.h"

/************************ Implementation Functions **************************/

// initialize note values
void initializeNote(volatile note_t* note, uint8_t pitch)
{
	note->velocity	= AMP_MAX;
	note->pitch = pitch;
	note->duty_cycle = DC_MAX / 2;
	note->note_on = true;

}

// Initialize sequencer values
void initialize_sequencer(sequencer_t* sequencer)
{
	initializeNote(&(sequencer->note[0]),  50);  	// Initialize to D dorian scale
	initializeNote(&(sequencer->note[1]),  52);
	initializeNote(&(sequencer->note[2]),  53);
	initializeNote(&(sequencer->note[3]),  55);
	initializeNote(&(sequencer->note[4]),  57);
	initializeNote(&(sequencer->note[5]),  59);
	initializeNote(&(sequencer->note[6]),  60);
	initializeNote(&(sequencer->note[7]),  62);
	initializeNote(&(sequencer->note[8]),  64);
	initializeNote(&(sequencer->note[9]),  65);
	initializeNote(&(sequencer->note[10]), 67);
	initializeNote(&(sequencer->note[11]), 69);
	initializeNote(&(sequencer->note[12]), 71);
	initializeNote(&(sequencer->note[13]), 72);
	initializeNote(&(sequencer->note[14]), 74);
	initializeNote(&(sequencer->note[15]), 76);

	  // Initialize states
	sequencer->step			= FIRST;                		// initialize sequence step to first
	sequencer->prevStep 	= FIRST;						// initialize previous sequencer step to first step
	sequencer->timer_count  = (TIMER_CLOCK / 8);    		// initialize timer count to blink 8x per second
	sequencer->seq_timer 	= (TIMER_CLOCK / 80) * 60;  	// initialize sequence rate to 80 BPM
	sequencer->led_toggle 	= false;           				// initialize LED to off
	sequencer->pattern 		= BOTH_DIR;           			// initialize sequence pattern to forward
	sequencer->subdiv 		= QUARTER;            			// initialize note subdivision to quarter notes

}

// increment sequence step (right)
static void stepForward(sequencer_t* sequencer)
{
	sequencer->step++;            // increment step
	if (sequencer->step > LAST)
	{
		sequencer->step = FIRST;    // wrap around from last step to first
	}
}

// decrement sequence step (left)
static void stepBackward(sequencer_t* sequencer)
{
	if (sequencer->step <= FIRST)
	{
		sequencer->step = LAST;     // wrap around from first step to last
	}
	else
	{
		sequencer->step--;          // decrement step
	}
}

// set sequence step to random step
static void stepRandom(sequencer_t* sequencer)
{
	sequencer->step = rand() % STEPS;    // randomize next step
}

// Determine which sequencer step follows the current step
void sequence_step(sequencer_t* sequencer)
{
	uint8_t step = sequencer->step;

	switch(sequencer->pattern)    // case statement to select next step based on pattern stored in states
	{
		case FORWARD:	stepForward(sequencer);
                    	break;

		case BACKWARD:	stepBackward(sequencer);
                    	break;

		case BOTH_DIR:	if (sequencer->step == FIRST)
						{
							stepForward(sequencer);
						}
						else if (sequencer->step == LAST)
						{
							stepBackward(sequencer);
						}
						else if (sequencer->prevStep < sequencer->step)
						{
							stepForward(sequencer);
						}
						else if (sequencer->prevStep > sequencer->step)
						{
							stepBackward(sequencer);
						}
						break;

		case RANDOM:    stepRandom(sequencer);
						break;

		default:        break;
	}

	sequencer->prevStep = step;

	return;
}


