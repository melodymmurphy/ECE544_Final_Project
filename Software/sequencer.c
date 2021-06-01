// Author: Mel Murphy

#include "sequencer.h"

/************************* Variable Declarations ****************************/

static sequencer_t* sequencer;						// Sequencer struct

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
void initialize_sequencer(sequencer_t* sequencer_p)
{

	xil_printf("\n\nInitializing Sequencer...\r\n");

	sequencer = sequencer_p;


	initializeNote(&sequencer->note[0],  50);  	// Initialize to D dorian scale
	initializeNote(&sequencer->note[1],  52);
	initializeNote(&sequencer->note[2],  53);
	initializeNote(&sequencer->note[3],  55);
	initializeNote(&sequencer->note[4],  57);
	initializeNote(&sequencer->note[5],  59);
	initializeNote(&sequencer->note[6],  60);
	initializeNote(&sequencer->note[7],  62);
	initializeNote(&sequencer->note[8],  64);
	initializeNote(&sequencer->note[9],  65);
	initializeNote(&sequencer->note[10], 67);
	initializeNote(&sequencer->note[11], 69);
	initializeNote(&sequencer->note[12], 71);
	initializeNote(&sequencer->note[13], 72);
	initializeNote(&sequencer->note[14], 74);
	initializeNote(&sequencer->note[15], 76);

	  // Initialize system states
	sequencer->step			= FIRST;                		// initialize sequence step to first
	sequencer->prevStep 	= FIRST;						// initialize previous sequencer step to first step
	sequencer->mode 		= RECORD;               		// initialize mode to RECORD mode
	sequencer->timer_count  = (TIMER_CLOCK / 8);    		// initialize timer count to blink 8x per second
	sequencer->seq_timer 	= (TIMER_CLOCK / 80) * 60;  	// initialize sequence rate to 80 BPM
	sequencer->led_toggle 	= false;           				// initialize LED to off
	sequencer->pattern 		= BOTH_DIR;           			// initialize sequence pattern to forward
	sequencer->subdiv 		= QUARTER;            			// initialize note subdivision to quarter notes

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

// play an audio signal corresponding to the current sequencer step
void play_note(signal_generator_t* sigGen)
{
	note_t note = sequencer->note[sequencer->step];
	sigGen->frequency[0] = note_to_freq(note.pitch);
	sigGen->velocity[0] = note.velocity;

	return;
}

// send a MIDI signal to play the current sequencer step
void send_note(void)
{
	note_t note = sequencer->note[sequencer->step];
	setMidiOutNote(CHANNEL_0, note.pitch, note.velocity);
	sendMidiNoteOn();
}

// send a MIDI signal to stop the current sequencer step
void stop_note(void)
{
	note_t note = sequencer->note[sequencer->step];
	setMidiOutNote(CHANNEL_0, note.pitch, note.velocity);
	sendMidiNoteOff();
}
