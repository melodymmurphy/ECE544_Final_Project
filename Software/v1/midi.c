// Author: Mel Murphy

#include "midi.h"

/************************* Variable Declarations ****************************/

midi_rx_t* midi_rx;
midi_tx_t* midi_tx;

/************************ Implementation Functions **************************/

void initialize_MIDI(midi_rx_t* midi_rx_p, midi_tx_t* midi_tx_p)
{
	// initialize MIDI receiver
	midi_rx				= midi_rx_p;
	midi_rx->active 	= 0;
	midi_rx->numActive 	= 0;
	for (int index = 0; index < NUM_INST; index++)
	{
		midi_rx->notesOn[index]  = 0;
		midi_rx->velocity[index] = 0;
	}
	midi_rx->modulation = 0;

	// initialize MIDI transmitter
	midi_tx 			= midi_tx_p;
	midi_tx->channel 	= CHANNEL_0;
	midi_tx->pitch 		= 0;
	midi_tx->velocity 	= 0;
	midi_tx->controller = 1;	// controller 1 is typically mod wheel
	midi_tx->modulation = 0;


	return;
}

// set parameters for note signal transmission
void setMidiOutNote(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
	midi_tx->channel  = channel;
	midi_tx->pitch	  = pitch;
	midi_tx->velocity = velocity;
}

// set parameters for modulation signal transmission
void setMidiOutMod(uint8_t channel, uint8_t controller, uint8_t modulation)
{
	midi_tx->channel	= channel;
	midi_tx->controller = controller;
	midi_tx->modulation = modulation;
}

// transmit note on signal
void sendMidiNoteOn(void)
{
	int rc;

	uint8_t status_byte = 0x90 | midi_tx->channel;
	uint8_t bytes[3] = {status_byte, midi_tx->pitch, midi_tx->velocity};

	rc = XUartNs550_Send(&UART_inst, bytes, 3);		// send through UART

	return;
}

// transmit note off signal
void sendMidiNoteOff(void)
{
	int rc;

	uint8_t status_byte = 0x80 | midi_tx->channel;
	uint8_t bytes[3] = {status_byte, midi_tx->pitch, midi_tx->velocity};

	rc = XUartNs550_Send(&UART_inst, bytes, 3);		// send through UART

	return;
}

// transmit modulation signal
void sendMidiModulation(void)
{
	int rc;
	uint8_t status_byte = 0xB0 | midi_tx->channel;
	uint8_t bytes[3] = {status_byte, midi_tx->controller, midi_tx->modulation};

	rc = XUartNs550_Send(&UART_inst, bytes, 3);		// send through UART

	return;
}

void setModulationRX(uint8_t modulation)
{
	midi_rx->modulation = modulation;

	return;
}

uint8_t getModulationRX(void)
{
	return midi_rx->modulation;
}

static void setActive(void)
{
	midi_rx->numActive = 0;
	for (int index = 0; index < NUM_INST; index++)
	{
		if (midi_rx->notesOn[index] > 0)
		{
			(midi_rx->numActive++);
		}
	}

	return;
}

void setNotes(void)
{
	uint32_t notes_lo = MIDI_processor_getNotesLow();
	uint32_t notes_hi = MIDI_processor_getNotesHigh();
	for (int index = 0; index < NUM_INST / 2; index++)
	{
		midi_rx->notesOn[index  ] = ((notes_lo & (0xFF << (8*index))) >> (8*index));
		midi_rx->notesOn[index+4] = ((notes_hi & (0xFF << (8*index))) >> (8*index));
	}
	setActive();
	return;
}

