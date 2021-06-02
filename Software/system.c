// Author: Mel Murphy

#include "system.h"

/************************* Variable Declarations ****************************/

static system_t* seq_system;

/************************ Implementation Functions **************************/

void initialize_system(system_t* system_p)
{
	xil_printf("\n\nInitializing System...\r\n");

	seq_system = system_p;

	for (int index = 0; index < BANK_SIZE; index++)
	{
		initialize_sequencer(&(seq_system->sequencer_bank[index]));
	}
	initializeSigGen(&(seq_system->sigGen));
	initialize_MIDI(&(seq_system->midi_rx), &(seq_system->midi_tx));
	seq_system->active_sequence = 0;
	seq_system->mode = RECORD;

	return;
}

seq_mode_t getMode(void)
{
	return seq_system->mode;
}

void setMode(seq_mode_t mode)
{
	seq_system->mode = mode;
	return;
}

void setSlot(uint8_t activeSequence)
{
	if ((activeSequence >= 0) && (activeSequence < BANK_SIZE))
	{
		seq_system->active_sequence = activeSequence;
	}

	return;
}

seq_mode_t getSlot(void)
{
	return seq_system->active_sequence;
}

void updateLEDs()
{
	uint32_t step_LED = (1 << (STEPS-1)) >> seq_system->sequencer_bank[seq_system->active_sequence].step;			// LED of active sequencer step
	XGpio_DiscreteClear(&GPIO_inst, CHANNEL_2, GPIO_LEDS_MASK);		// clear all LEDs
	XGpio_DiscreteSet(&GPIO_inst, CHANNEL_2, step_LED);				// write just the LED we want

}

// play an audio signal corresponding to the current sequencer step
void play_note()
{
	sequencer_t sequencer = seq_system->sequencer_bank[seq_system->active_sequence];
	note_t note = sequencer.note[sequencer.step];
	seq_system->sigGen.frequency[0] = note_to_freq(note.pitch);
	seq_system->sigGen.velocity[0] = note.velocity;

	return;
}

// send a MIDI signal to play the current sequencer step
void send_note()
{
	sequencer_t sequencer = seq_system->sequencer_bank[seq_system->active_sequence];
	note_t note = sequencer.note[sequencer.step];
	setMidiOutNote(CHANNEL_0, note.pitch, note.velocity);
	sendMidiNoteOn();
}

// send a MIDI signal to stop the current sequencer step
void stop_note()
{
	sequencer_t sequencer = seq_system->sequencer_bank[seq_system->active_sequence];
	note_t note = sequencer.note[sequencer.step];
	setMidiOutNote(CHANNEL_0, note.pitch, note.velocity);
	sendMidiNoteOff();
}

void next_step()
{
	sequencer_t* sequencer = &(seq_system->sequencer_bank[seq_system->active_sequence]);
	sequence_step(sequencer);

}
