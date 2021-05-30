// Author: Mel Murphy

#include "sequencer.h"

/************************* Variable Declarations ****************************/

static sequencer_t* sequencer;						// Sequencer struct

/************************ Implementation Functions **************************/

void initialize_sequencer(sequencer_t* sequencer_p)
{

	xil_printf("\n\nInitializing Sequencer...\r\n");

	sequencer = sequencer_p;

	for (int step = 0; step < STEPS; step++)
	{
		initializeNote(&(sequencer->note[step]));  	// initialize notes of sequence
	}

	  // Initialize system states
	sequencer->step = FIRST;                			// initialize sequence step to first
	sequencer->prevStep = FIRST;						// initialize previous sequencer step to first step
	sequencer->mode = RECORD;               			// initialize mode to RECORD mode
	sequencer->timer_count = (TIMER_CLOCK / 8);    	// initialize timer count to blink 8x per second
	sequencer->seq_timer = (TIMER_CLOCK / 80) * 60;  // initialize sequence rate to 80 BPM
	sequencer->led_toggle = false;           		// initialize LED to off
	sequencer->pattern = FORWARD;           			// initialize sequence pattern to forward
	sequencer->subdiv = QUARTER;            			// initialize note subdivision to quarter notes

}

// increment sequence step (right)
static void stepForward(void)
{
	sequencer->step++;            // increment step
	if (sequencer->step > LAST)
	{
		sequencer->step = FIRST;    // wrap around from last step to first
	}
}

// decrement sequence step (left)
static void stepBackward(void)
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
static void stepRandom(void)
{
	sequencer->step = rand() % STEPS;    // randomize next step
}

// Determine which sequencer step follows the current step
void next_step(void)
{
	uint8_t step = sequencer->step;

	switch(sequencer->pattern)    // case statement to select next step based on pattern stored in system states
	{
		case FORWARD:	stepForward();
                    	break;

		case BACKWARD:	stepBackward();
                    	break;

		case BOTH_DIR:	if (sequencer->step == FIRST)
						{
							stepForward();
						}
						else if (sequencer->step == LAST)
						{
							stepBackward();
						}
						else if (sequencer->prevStep < sequencer->step)
						{
							stepForward();
						}
						else if (sequencer->prevStep > sequencer->step)
						{
							stepBackward();
						}
						break;

		case RANDOM:    stepRandom();
						break;

		default:        break;
	}

	sequencer->prevStep = step;

	return;
}
