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
		initialize_sequencer(&(seq_system->active_sequence));
	}
	initializeSigGen(&(seq_system->sigGen));
	initialize_MIDI(&(seq_system->midi_rx), &(seq_system->midi_tx));
	seq_system->slot = 1;
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

uint8_t getSlot(void)
{
	return seq_system->slot;
}

void setSlot(uint8_t slot)
{
	if ((slot >= 1) && (slot <= 16))
	{
		seq_system->slot = slot;
	}
	return;
}

void store_sequence(void)
{
	seq_system->sequencer_bank[seq_system->slot] = seq_system->active_sequence;
	return;
}

void load_sequence(void)
{
	seq_system->active_sequence = seq_system->sequencer_bank[seq_system->slot];
	return;
}

void updateLEDs(void)
{
	uint32_t step_LED = (1 << (STEPS-1)) >> seq_system->active_sequence.step;			// LED of active sequencer step
	XGpio_DiscreteClear(&GPIO_inst, CHANNEL_2, GPIO_LEDS_MASK);		// clear all LEDs
	XGpio_DiscreteSet(&GPIO_inst, CHANNEL_2, step_LED);				// write just the LED we want

}

// play an audio signal corresponding to the current sequencer step
void play_note(void)
{
	sequencer_t sequencer = seq_system->active_sequence;
	note_t note = sequencer.note[sequencer.step];
	seq_system->sigGen.frequency[0] = note_to_freq(note.pitch);
	seq_system->sigGen.velocity[0] = note.velocity;

	return;
}

// send a MIDI signal to play the current sequencer step
void send_note(void)
{
	sequencer_t sequencer = seq_system->active_sequence;
	note_t note = sequencer.note[sequencer.step];
	setMidiOutNote(CHANNEL_0, note.pitch, note.velocity);
	sendMidiNoteOn();
}

// send a MIDI signal to stop the current sequencer step
void stop_note(void)
{
	sequencer_t sequencer = seq_system->active_sequence;
	note_t note = sequencer.note[sequencer.step];
	setMidiOutNote(CHANNEL_0, note.pitch, note.velocity);
	sendMidiNoteOff();
}

void next_step(void)
{
	sequencer_t* sequencer = &(seq_system->active_sequence);
	sequence_step(sequencer);

}

void setVolume(uint8_t volume)
{
	if ((volume >= VOLUME_MIN) && (volume <= VOLUME_MAX))
	{
		seq_system->volume = volume;
	}
	return;
}

uint8_t getVolume(void)
{
	return seq_system->volume;
}

void setTempo(uint8_t tempo)
{
	uint64_t timer_count;
	if ((tempo >= TEMPO_MIN) && (tempo <= TEMPO_MAX))
	{
		seq_system->active_sequence.tempo = tempo;
		timer_count = ((AXI_CLOCK_FREQ_HZ * 60) / tempo);
		seq_system->active_sequence.timer_count = (uint32_t)timer_count;
		XTmrCtr_SetLoadReg(AXI_TIMER_BASEADDR, TIMER_1 , (uint32_t)timer_count);
		XTmrCtr_LoadTimerCounterReg(AXI_TIMER_BASEADDR, TIMER_1);
	}

	return;
}

uint8_t getTempo(void)
{
	return seq_system->active_sequence.tempo;
}

void setSubdiv(note_length_t subdiv)
{
	seq_system->active_sequence.subdiv = subdiv;
	return;
}

note_length_t getSubdiv(void)
{
	return seq_system->active_sequence.subdiv;
}

void setSwing(uint8_t swing)
{
	if ((swing >= SWING_MIN) && (swing <= SWING_MAX))
	{
		seq_system->active_sequence.swing = swing;
	}

	return;
}

uint8_t getSwing(void)
{
	return seq_system->active_sequence.swing;
}

void setPattern(pattern_t pattern)
{
	seq_system->active_sequence.pattern = pattern;
	return;
}

pattern_t getPattern(void)
{
	return seq_system->active_sequence.pattern;
}

void setNoteVelocity(uint8_t velocity)
{
	if ((velocity >= VELOCITY_MIN) && (velocity <= VELOCITY_MAX))
	{
		seq_system->active_sequence.note[seq_system->active_sequence.step].velocity = velocity;
	}

	return;
}

void setNotePitch(uint8_t pitch)
{
	if ((pitch >= PITCH_MIN) && (pitch <= PITCH_MAX))
	{
		seq_system->active_sequence.note[seq_system->active_sequence.step].pitch = pitch;
	}

	return;
}

void setDutyCycle(uint8_t duty_cycle)
{
	if ((duty_cycle >= DC_MIN) && (duty_cycle <= DC_MAX))
	{
		seq_system->active_sequence.note[seq_system->active_sequence.step].duty_cycle = duty_cycle;
	}
	return;
}

void setState(bool note_on)
{
	seq_system->active_sequence.note[seq_system->active_sequence.step].note_on = note_on;
	return;
}
