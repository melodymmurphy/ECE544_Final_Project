/**
*
* @file system.c
*
* @author Mel Murphy (cumurphy@pdx.edu)
*
* System level functions. Note that this file grew WAY PAST its intended size.
* Many of the implementation functions should be move to their respective
* modules, but the time crunch scattered our organization. I'll clean this up
* for future use.
*
******************************************************************************/

/****************************** Includes *********************************/

#include "system.h"

//************************* Variable Declarations ****************************/

static system_t* seq_system;

/************************ Implementation Functions **************************/

// Initialize system parameters and modules
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
	seq_system->slot = 0;
	seq_system->volume = 50;
	seq_system->led_toggle = false;
	seq_system->slot = 1;
	seq_system->mode = RECORD;
	seq_system->memory_index = 0;
	seq_system->byte_counter = 0;
	seq_system->tune_byte = 0;

	return;
}

// get current system mode
seq_mode_t getMode(void)
{
	return seq_system->mode;
}

// set current system mode
void setMode(seq_mode_t mode)
{
	uint32_t timer_count;
	if (mode == PLAY)
	{
		quiet_note(0);	// silence voices 0, 1, and 2
		quiet_note(1);
		quiet_note(2);
		timer_count = tempo_timer(&(seq_system->active_sequence), getTempo());
		seq_system->led_toggle = true;
		updateLEDs();
		XTmrCtr_Stop(&AXITimerInst, TIMER_1);		// stop sequence timer
		XTmrCtr_SetResetValue(&AXITimerInst, TIMER_1, timer_count);	// load sequence timer with LED blink rate
		XTmrCtr_Reset(&AXITimerInst, TIMER_1);		// reset the timer
		XTmrCtr_Start(&AXITimerInst, TIMER_1);
		play_note();	// update current note frequences in signal generator so note will play
		send_note();	// send MIDI ON for current MIDI note
	}

	if (mode == RECORD)
	{
		stop_note();	// send NOTE OFF message for current MIDI note
		quiet_note(0);	// silence voices 0, 1, and 2
		quiet_note(1);
		quiet_note(2);
		XTmrCtr_Stop(&AXITimerInst, TIMER_1);		// stop sequence timer
		XTmrCtr_SetResetValue(&AXITimerInst, TIMER_1, RECORD_BLINK_RATE);	// load sequence timer with LED blink rate
		XTmrCtr_Reset(&AXITimerInst, TIMER_1);		// reset the timer
		XTmrCtr_Start(&AXITimerInst, TIMER_1);
		seq_system->led_toggle = false;
		updateLEDs();
	}
	if (mode == BYPASS)
	{
		stop_note();	// send NOTE OFF message for current MIDI note
		quiet_note(0);  // silence voices 0, 1, and 2
		quiet_note(1);
		quiet_note(2);
		XTmrCtr_Stop(&AXITimerInst, TIMER_1);		// stop sequence timer
		XTmrCtr_SetResetValue(&AXITimerInst, TIMER_1, BYPASS_BLINK_RATE);	// load sequence timer with LED blink rate
		XTmrCtr_Reset(&AXITimerInst, TIMER_1);		// reset the timer
		XTmrCtr_Start(&AXITimerInst, TIMER_1);
		seq_system->led_toggle = false;
		updateLEDs();
	}
	if (mode == MEMORY)
	{
		stop_note();	// send NOTE OFF message for current MIDI note
		quiet_note(0);	 // silence voices 0, 1, and 2
		quiet_note(1);
		quiet_note(2);
	}

	seq_system->mode = mode;	// update mode in system struct

	return;
}

// get currently selected memory slot in menu
uint8_t getSlot(void)
{
	return seq_system->slot;
}

// set currently selected memory slot in menu
void setSlot(uint8_t slot)
{
	if ((slot >= 1) && (slot <= 16))
	{
		seq_system->slot = slot;
	}
	return;
}

// store sequence to memory
void store_sequence(void)
{
	seq_system->sequencer_bank[seq_system->slot - 1] = seq_system->active_sequence;
	return;
}

// read sequence from memory
void load_sequence(void)
{
	seq_system->active_sequence = seq_system->sequencer_bank[seq_system->slot - 1];
	return;
}

// update LEDs on board
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

// play an audio signal with voice 0 corresponding to sequence step or note memory
void play_note(void)
{
	sequencer_t sequencer = seq_system->active_sequence;
	note_t note = sequencer.note[sequencer.step];
	if (getMode() == PLAY)
	{
		uint16_t switches = getSwitches();
		if (switches & (1 << (LAST - sequencer.step)))
		{
			seq_system->sigGen.frequency[0] = note_to_freq(note.pitch);
			seq_system->sigGen.velocity[0] = note.velocity;
		}
	}
	else if (getMode() == MEMORY)
	{
		seq_system->tune_byte = readNextNote();
		seq_system->sigGen.frequency[0] = note_to_freq(seq_system->tune_byte);
		seq_system->sigGen.velocity[0] = VELOCITY_MAX / 2;
	}

	return;
}

// Turn off currently playing note
void quiet_note(uint8_t index)
{
	seq_system->sigGen.frequency[index] = 0;
	seq_system->sigGen.velocity[index] = 0;

	return;
}

// send a MIDI signal to play the current sequencer step
void send_note(void)
{
	sequencer_t sequencer = seq_system->active_sequence;
	note_t note = sequencer.note[sequencer.step];
	if (getMode() == PLAY)
	{
		setMidiOutNote(CHANNEL_1, note.pitch, note.velocity);
	}
	else if (getMode() == MEMORY)
	{
		setMidiOutNote(CHANNEL_1, seq_system->tune_byte, VELOCITY_MAX);
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
		setMidiOutNote(CHANNEL_1, note.pitch, note.velocity);
	}
	else if (getMode() == MEMORY)
	{
		setMidiOutNote(CHANNEL_1, seq_system->tune_byte, note.velocity);
	}
	sendMidiNoteOff();
}

// move to next sequence step
void next_step(void)
{
	sequencer_t* sequencer = &(seq_system->active_sequence);
	mode_t mode = getMode();
	if (mode == PLAY)
	{
		sequence_step(sequencer);
	}
	else if (mode == RECORD)
	{
		(sequencer->step)++;
		if (sequencer->step > LAST)
		{
			sequencer->step = FIRST;
		}
	}


	return;
}

// set system volume
void setVolume(uint8_t volume)
{
	if ((volume >= VOLUME_MIN) && (volume <= VOLUME_MAX))
	{
		seq_system->volume = volume;
	}
	return;
}

// get system volume value
uint8_t getVolume(void)
{
	return seq_system->volume;
}

// set current sequence's tempo
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

// get current sequence's tempo
uint8_t getTempo(void)
{
	return seq_system->active_sequence.tempo;
}

// set current sequence note subdivision
void setSubdiv(note_length_t subdiv)
{
	uint32_t timer_count;
	seq_system->active_sequence.subdiv = subdiv;
	uint32_t current_value = XTmrCtr_GetValue(&AXITimerInst, TIMER_1);
	timer_count = tempo_timer(&(seq_system->active_sequence), seq_system->active_sequence.tempo);	// update timer

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

// get current sequence note subdivision value
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

// get current sequence swing value
int8_t getSwing(void)
{
	return seq_system->active_sequence.swing;
}

// set current sequence pattern
void setPattern(pattern_t pattern)
{
	seq_system->active_sequence.pattern = pattern;
	return;
}

// get current sequence pattern
pattern_t getPattern(void)
{
	return seq_system->active_sequence.pattern;
}

// set velocity of currently selected note
void setNoteVelocity(uint8_t velocity)
{
	if ((velocity >= VELOCITY_MIN) && (velocity <= VELOCITY_MAX))
	{
		seq_system->active_sequence.note[seq_system->active_sequence.step].velocity = velocity;
	}

	return;
}

// set pitch of currently selected note
void setNotePitch(uint8_t pitch)
{
	if ((pitch >= (PITCH_MIN + 1)) && (pitch <= PITCH_MAX))
	{
		seq_system->active_sequence.note[seq_system->active_sequence.step].pitch = pitch;
	}

	return;
}

// set duty cycle of currently selected note
void setDutyCycle(uint8_t duty_cycle)
{
	if ((duty_cycle >= DC_MIN) && (duty_cycle <= DC_MAX))
	{
		seq_system->active_sequence.note[seq_system->active_sequence.step].duty_cycle = duty_cycle;
	}
	return;
}

// set note on/off state of currently selected note
void setState(bool note_on)
{
	seq_system->active_sequence.note[seq_system->active_sequence.step].note_on = note_on;
	return;
}

// step the sequence to the right by one
void stepSeqForward(void)
{
	stepForward(&(seq_system->active_sequence));
	seq_system->led_toggle = true;
	return;
}

// step the sequence to the left by one
void stepSeqBackward(void)
{
	stepBackward(&(seq_system->active_sequence));
	seq_system->led_toggle = true;
	return;
}

// toggle LED blink value on/off
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

// get currently active note in selected spot
uint8_t getNote(uint8_t index)
{
	return seq_system->midi_rx.notesOn[index];
}

// read all currently active notes into an array (for debugging purposes)
void getNotes(uint8_t* noteArray)
{
	for (int index = 0; index < NUM_INST; index++)
	{
		noteArray[index] = getNote(index);
	}
	return;
}

// get number of currently active notes in MIDI processor
uint8_t getMidiActive(void)
{
	return seq_system->midi_rx.numActive;
}

// get frequencies of currently active MIDI notes and write them to the signal generator slots
void getFreqs(void)
{
	mode_t mode = getMode();
	if (mode == BYPASS || mode == RECORD)	// in BYPASS or RECORD mode, play to all three voices
	{
		setPitch(0, seq_system->midi_rx.notesOn[7]);
		setPitch(1, seq_system->midi_rx.notesOn[6]);
		setPitch(2, seq_system->midi_rx.notesOn[5]);
	}
	else if (mode == PLAY || mode == MEMORY)	// in PLAY or MEMORY mode, play to just two voices--one is
	{											//  already used by the currently playing sequence
		setPitch(1, seq_system->midi_rx.notesOn[7]);
		setPitch(2, seq_system->midi_rx.notesOn[6]);
	}
	return;
}

// compute constants used for waveform generation--moved to different task so that it runs once at each frequency
//  change instead of running at the beginning of every wavecycle. This is to optimize the system for processing power.
//	However, it's a stopgap--really this needs to be done in hardware to free up system resources. Alternatively, I could
//	figure out how to add more processors. Or move to something more powreful than the microblaze
void compute_constants(uint8_t signalIndex)
{
	if (seq_system->sigGen.frequency[signalIndex] > 0)
	{
		seq_system->sigGen.peak[signalIndex] =  0x800000 + 27962 * seq_system->volume;		// peak volume divided by max volume parameter
		seq_system->sigGen.cycle[signalIndex] = (SAMPLE_RATE << 1 ) / seq_system->sigGen.frequency[signalIndex];
		seq_system->sigGen.high_cycle[signalIndex] = (seq_system->sigGen.riseCycle[signalIndex] * seq_system->sigGen.cycle[signalIndex]) / RC_MAX;	// length of high part of duty cycle
		seq_system->sigGen.low_cycle[signalIndex] = seq_system->sigGen.cycle[signalIndex] - seq_system->sigGen.high_cycle[signalIndex];
		seq_system->sigGen.peak[signalIndex] =  0x800000 + 27962 * seq_system->volume;		// peak volume divided by max volume parameter
		seq_system->sigGen.rampUp[signalIndex] = seq_system->sigGen.peak[signalIndex] / seq_system->sigGen.high_cycle[signalIndex];
		seq_system->sigGen.rampDown[signalIndex] = seq_system->sigGen.peak[signalIndex] / seq_system->sigGen.low_cycle[signalIndex];
	}

	return;
}

void seq_timer(void)
{
	uint32_t timer_count = tempo_timer(&(seq_system->active_sequence), seq_system->active_sequence.tempo);
	XTmrCtr_SetResetValue(&AXITimerInst, TIMER_1, timer_count);	// load sequence timer with LED blink rate
	XTmrCtr_Reset(&AXITimerInst, TIMER_1);		// reset the timer

	return;
}

// read next note from BRAM block which has MIDI data stored to it. At this time, it's Bach's Praeludium no. 1 in C major.
//	One of my favorite songs to play on piano :)
uint8_t readNextNote(void)
{
	uint32_t midi_word = XBram_ReadReg(MIDI_BRAM_BASEADDR, seq_system->memory_index);	// read word of data from memory
	uint8_t midi_note = (midi_word & (0xFF000000 >> (seq_system->byte_counter * 8))) >> ((3 - seq_system->byte_counter) * 8);	// notes storage is big endian

	seq_system->byte_counter = seq_system->byte_counter + 1;	// increment byte we're reading from word
	if (seq_system->byte_counter == 4)
	{
		seq_system->byte_counter = 0;	// roll over byte counter at new word
		seq_system->memory_index += 4;	// if we're at the last byte, increment the word we're reading
	}

	if (seq_system->memory_index > (TUNE_LENGTH * 4))
	{
		seq_system->memory_index = 0;	// reset sequence once we're past the end
		seq_system->byte_counter = 0;
	}

	return midi_note;
}

// debugging function to read out all the frequencies in the signal generator
void readOutFreqs(void)
{
	for (int index = 0; index < NUM_POLY; index++)
	{
		xil_printf("frequency at index %d: %d\r\n", index, (int)seq_system->sigGen.frequency[index]);
	}
}

// get number of currently active voices in the signal generator (i swear i have another function exactly like this somewhere)
uint8_t getActiveVoices(void)
{
	uint8_t voices = 0;
	for (int index = 0; index < NUM_POLY; index++)
	{
		if (seq_system->sigGen.frequency[index] > 0)
		{
			voices++;
		}
	}
	return voices;
}

// Set desired voice's rise cycle to the modulation value
void setMod(uint8_t index, uint8_t modulation)
{
	seq_system->sigGen.riseCycle[index] = modulation;
}
