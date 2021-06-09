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
	sequencer->direction	= RIGHT;						// initialize sequener direction to moving forward
	sequencer->tempo 		= 80;							// initialize tempo to 80 BPM
	sequencer->swing 		= 0;							// initialize sequencer to no swing
	sequencer->pattern 		= BOTH_DIR;           			// initialize sequence pattern to forward
	sequencer->subdiv 		= QUARTER;            			// initialize note subdivision to quarter notes

}

// increment sequence step (right)
void stepForward(sequencer_t* sequencer)
{
	sequencer->step++;            // increment step
	if (sequencer->step > LAST)
	{
		sequencer->step = FIRST;    // wrap around from last step to first
	}
}

// decrement sequence step (left)
void stepBackward(sequencer_t* sequencer)
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

	switch(sequencer->pattern)    // case statement to select next step based on pattern stored in states
	{
		case FORWARD:	stepForward(sequencer);
                    	break;

		case BACKWARD:	stepBackward(sequencer);
                    	break;

		case BOTH_DIR:	if (sequencer->step == FIRST)
						{
							stepForward(sequencer);
							sequencer->direction = RIGHT;
						}
						else if (sequencer->step == LAST)
						{
							stepBackward(sequencer);
							sequencer->direction = LEFT;
						}
						else if (sequencer->direction == RIGHT)
						{
							stepForward(sequencer);
						}
						else if (sequencer->direction == LEFT)
						{
							stepBackward(sequencer);
						}

						break;

		case RANDOM:    stepRandom(sequencer);
						break;

		default:        break;
	}

	return;
}

uint32_t tempo_timer(sequencer_t* sequencer, uint8_t tempo)
{
	uint32_t timer_count;
	if ((sequencer->step % 2) == 0)
	{
		timer_count = (TIMER_CLOCK / tempo) * (SWING_MAX + sequencer->swing);
		timer_count /= SWING_MAX;
		timer_count = (timer_count >> (sequencer->subdiv)) + 1;	// add one to avoid 0 timer cases
	}
	else
	{
		timer_count = (TIMER_CLOCK / tempo) * (SWING_MAX - sequencer->swing);
		timer_count /= SWING_MAX;
		timer_count = (timer_count >> (sequencer->subdiv)) + 1;
	}
	timer_count *= 60;
	return timer_count;
}
