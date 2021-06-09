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
	seq_system->led_toggle = false;
	seq_system->slot = 1;
	seq_system->mode = RECORD;
	seq_system->memory_index = 0;
	seq_system->byte_counter = 0;
	seq_system->tune_byte = 0;

	return;
}

seq_mode_t getMode(void)
{
	return seq_system->mode;
}

void setMode(seq_mode_t mode)
{
	uint32_t timer_count;
	if (mode == PLAY)
	{
		timer_count = tempo_timer(&(seq_system->active_sequence), getTempo());
		seq_system->led_toggle = true;
		updateLEDs();
		XTmrCtr_Stop(&AXITimerInst, TIMER_1);		// stop sequence timer
		XTmrCtr_SetResetValue(&AXITimerInst, TIMER_1, timer_count);	// load sequence timer with LED blink rate
		XTmrCtr_Reset(&AXITimerInst, TIMER_1);		// reset the timer
		XTmrCtr_Start(&AXITimerInst, TIMER_1);
		play_note();
		send_note();
	}

	if (mode == RECORD)
	{
		stop_note();
		quiet_note();		// turn of currently playing sound
		XTmrCtr_Stop(&AXITimerInst, TIMER_1);		// stop sequence timer
		XTmrCtr_SetResetValue(&AXITimerInst, TIMER_1, RECORD_BLINK_RATE);	// load sequence timer with LED blink rate
		XTmrCtr_Reset(&AXITimerInst, TIMER_1);		// reset the timer
		XTmrCtr_Start(&AXITimerInst, TIMER_1);
		seq_system->led_toggle = false;
		updateLEDs();
	}
	if (mode == BYPASS)
	{
		stop_note();
		quiet_note();		// turn of currently playing sound
		XTmrCtr_Stop(&AXITimerInst, TIMER_1);		// stop sequence timer
		XTmrCtr_SetResetValue(&AXITimerInst, TIMER_1, BYPASS_BLINK_RATE);	// load sequence timer with LED blink rate
		XTmrCtr_Reset(&AXITimerInst, TIMER_1);		// reset the timer
		XTmrCtr_Start(&AXITimerInst, TIMER_1);
		seq_system->led_toggle = false;
		updateLEDs();
	}

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
	uint16_t switches = getSwitches();
	XGpio_DiscreteClear(&GPIO_inst, CHANNEL_2, GPIO_LEDS_MASK);		// clear all LEDs
	if ((seq_system->mode == PLAY) && (switches & (1 << (LAST - seq_system->active_sequence.step))))
	{
		XGpio_DiscreteSet(&GPIO_inst, CHANNEL_2, step_LED);
	}
	if ((seq_system->mode == RECORD) && (seq_system->led_toggle))
	{
		XGpio_DiscreteSet(&GPIO_inst, CHANNEL_2, step_LED);				// write just the LED we want
	}
	if ((seq_system->mode == BYPASS) && (seq_system->led_toggle))
	{
		XGpio_DiscreteSet(&GPIO_inst, CHANNEL_2, GPIO_LEDS_MASK);				// write all LEDs
	}

}

// play an audio signal
void play_note(void)
{
	sequencer_t sequencer = seq_system->active_sequence;
	note_t note = sequencer.note[sequencer.step];
	if (getMode() == PLAY)
	{
		uint16_t switches = getSwitches();
		if (switches & (1 << (LAST - sequencer.step)))
		{
			seq_system->sigGen.frequency[7] = note_to_freq(note.pitch);
			seq_system->sigGen.velocity[7] = note.velocity;
		}
	}
	else if (getMode() == MEMORY)
	{
		seq_system->tune_byte = readNextNote();
		seq_system->sigGen.frequency[7] = note_to_freq(seq_system->tune_byte);
		seq_system->sigGen.velocity[7] = VELOCITY_MAX / 2;
	}

	return;
}

// Turn off currently playing note
void quiet_note(void)
{
	sequencer_t sequencer = seq_system->active_sequence;
	seq_system->sigGen.frequency[7] = 0;
	seq_system->sigGen.velocity[7] = 0;

	return;
}

// send a MIDI signal to play the current sequencer step
void send_note(void)
{
	sequencer_t sequencer = seq_system->active_sequence;
	note_t note = sequencer.note[sequencer.step];
	if (getMode() == PLAY)
	{
		setMidiOutNote(CHANNEL_0, note.pitch, note.velocity);
	}
	else if (getMode() == MEMORY)
	{
		setMidiOutNote(CHANNEL_0, seq_system->tune_byte, VELOCITY_MAX / 2);
	}
	sendMidiNoteOn();
}

// send a MIDI signal to stop the current sequencer step
void stop_note(void)
{
	sequencer_t sequencer = seq_system->active_sequence;
	note_t note = sequencer.note[sequencer.step];
	if (getMode() == PLAY)
	{
		setMidiOutNote(CHANNEL_0, note.pitch, note.velocity);
	}
	else if (getMode() == MEMORY)
	{
		setMidiOutNote(CHANNEL_0, seq_system->tune_byte, note.velocity);
	}
	sendMidiNoteOff();
}

void next_step(void)
{
	sequencer_t* sequencer = &(seq_system->active_sequence);
	uint8_t midi_note;
	if (getMode() == PLAY)
	{
		sequence_step(sequencer);
	}
	else if (getMode() == RECORD)
	{
		(sequencer->step)++;
		if (sequencer->step > LAST)
		{
			sequencer->step = FIRST;
		}
	}


	return;
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
	uint32_t current_value = XTmrCtr_GetValue(&AXITimerInst, TIMER_1);
	uint32_t timer_count;
	if ((tempo >= TEMPO_MIN) && (tempo <= TEMPO_MAX))
	{
		timer_count = tempo_timer(&(seq_system->active_sequence), tempo);
		seq_system->active_sequence.tempo = tempo;
		if (getMode() == PLAY)
		{
			XTmrCtr_SetResetValue(&AXITimerInst, TIMER_1, timer_count);	// load sequence timer with LED blink rate
			if (timer_count < current_value)
			{
				XTmrCtr_Reset(&AXITimerInst, TIMER_1);		// reset the timer
			}
		}
	}

	return;
}

uint8_t getTempo(void)
{
	return seq_system->active_sequence.tempo;
}

void setSubdiv(note_length_t subdiv)
{
	uint32_t timer_count;
	seq_system->active_sequence.subdiv = subdiv;
	uint32_t current_value = XTmrCtr_GetValue(&AXITimerInst, TIMER_1);
	timer_count = tempo_timer(&(seq_system->active_sequence), seq_system->active_sequence.tempo);

	seq_system->active_sequence.subdiv = subdiv;
	if (getMode() == PLAY)
	{
		XTmrCtr_SetResetValue(&AXITimerInst, TIMER_1, timer_count);	// load sequence timer with LED blink rate
		if (timer_count < current_value)
		{
			XTmrCtr_Reset(&AXITimerInst, TIMER_1);		// reset the timer
		}
	}
	return;
}

note_length_t getSubdiv(void)
{
	return seq_system->active_sequence.subdiv;
}

void setSwing(int8_t swing)
{
	if ((swing >= SWING_MIN) && (swing <= SWING_MAX))
	{
		seq_system->active_sequence.swing = swing;
	}

	return;
}

int8_t getSwing(void)
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
	if ((pitch >= (PITCH_MIN + 1)) && (pitch <= PITCH_MAX))
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

void stepSeqForward(void)
{
	stepForward(&(seq_system->active_sequence));
	seq_system->led_toggle = true;
	return;
}

void stepSeqBackward(void)
{
	stepBackward(&(seq_system->active_sequence));
	seq_system->led_toggle = true;
	return;
}

void toggleBlink(void)
{
	if (seq_system->led_toggle)
	{
		seq_system->led_toggle = false;
	}
	else
	{
		seq_system->led_toggle = true;
	}
	return;
}

uint8_t getNote(uint8_t index)
{
	return seq_system->midi_rx.notesOn[index];
}

void getNotes(uint8_t* noteArray)
{
	for (int index = 0; index < NUM_INST; index++)
	{
		noteArray[index] = getNote(index);
	}
	return;
}

uint8_t getActive(void)
{
	return seq_system->midi_rx.numActive;
}


void getFreqs(void)
{
	for (int index = 0; index < NUM_INST; index++)
	{
		setPitch(index, seq_system->midi_rx.notesOn[index]);
	}
	return;
}

void compute_constants(uint8_t signalIndex)
{
	seq_system->sigGen.peak[signalIndex] = (seq_system->sigGen.velocity[signalIndex] * ((1 << BIT_DEPTH) - 1)) / AMP_MAX;
	if (seq_system->sigGen.frequency[signalIndex] > 0)
	{
		seq_system->sigGen.cycle[signalIndex] = (SAMPLE_RATE << 1 ) / seq_system->sigGen.frequency[signalIndex];
	}
	seq_system->sigGen.high_cycle[signalIndex] = (seq_system->sigGen.riseCycle[signalIndex] * seq_system->sigGen.cycle[signalIndex]) / RC_MAX;	// length of high part of duty cycle
	seq_system->sigGen.low_cycle[signalIndex] = seq_system->sigGen.cycle[signalIndex] - seq_system->sigGen.high_cycle[signalIndex];
	seq_system->sigGen.rampUp[signalIndex] = seq_system->sigGen.peak[signalIndex] / seq_system->sigGen.high_cycle[signalIndex];
	seq_system->sigGen.rampDown[signalIndex] = seq_system->sigGen.peak[signalIndex] / seq_system->sigGen.low_cycle[signalIndex];
	seq_system->sigGen.c[signalIndex] = 0;

	return;
}

void seq_timer(void)
{
	uint32_t timer_count = tempo_timer(&(seq_system->active_sequence), seq_system->active_sequence.tempo);
	XTmrCtr_SetResetValue(&AXITimerInst, TIMER_1, timer_count);	// load sequence timer with LED blink rate
	XTmrCtr_Reset(&AXITimerInst, TIMER_1);		// reset the timer

	return;
}

uint8_t readNextNote(void)
{
	uint32_t midi_word = XBram_ReadReg(MIDI_BRAM_BASEADDR, seq_system->memory_index);
	uint8_t midi_note = (midi_word & (0xFF000000 >> (seq_system->byte_counter * 8))) >> ((3 - seq_system->byte_counter) * 8);

	seq_system->byte_counter = seq_system->byte_counter + 1;
	if (seq_system->byte_counter == 4)
	{
		seq_system->byte_counter = 0;
		seq_system->memory_index += 4;
	}

	if (seq_system->memory_index > (TUNE_LENGTH * 4))
	{
		seq_system->memory_index = 0;
		seq_system->byte_counter = 0;
	}

	return midi_note;
}

