/**
*
* @file midi.c
*
* @author Mel Murphy (cumurphy@pdx.edu)
*
* MIDI receiver and transmitter implementation
*
******************************************************************************/

/****************************** Includes *********************************/

#include "midi.h"

/************************* Variable Declarations ****************************/

midi_rx_t* midi_rx;
midi_tx_t* midi_tx;

/************************ Implementation Functions **************************/

// Initialize MIDI functionality
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
	midi_tx->channel 	= CHANNEL_1;
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

// Transmit NOTE ON signal. A NOTE ON signal is three bytes. First is a status byte of 0x9X, with X equal to the
//	channel transmitted on plus one (channel 1 corresponds to index 0, etc.). Second is the note value (0-127), and
//	third is the velocity (0-127)
void sendMidiNoteOn(void)
{
	int rc;

	uint8_t status_byte = 0x90 | midi_tx->channel;
	uint8_t bytes[3] = {status_byte, midi_tx->pitch, midi_tx->velocity};

	rc = XUartNs550_Send(&UART_inst, bytes, 3);		// send through UART

	return;
}

// Transmit NOTE OFF signal. A NOTE ON signal is three bytes. First is a status byte of 0x8X, with X equal to the
//	channel transmitted on plus one (channel 1 corresponds to index 0, etc.). Second is the note value (0x00 - 0x7F), and
//	third is the velocity (0x0 - 0x7F)
void sendMidiNoteOff(void)
{
	int rc;

	uint8_t status_byte = 0x80 | midi_tx->channel;
	uint8_t bytes[3] = {status_byte, midi_tx->pitch, midi_tx->velocity};

	rc = XUartNs550_Send(&UART_inst, bytes, 3);		// send through UART

	return;
}

// Transmit NOTE OFF signal. A modulation signal is three bytes. First is a status byte of 0xBX, with X equal to the
//	channel transmitted on plus one (channel 1 corresponds to index 0, etc.). Second is the controller value (0x00 - 0x7F),
//	and third is the parameter value (0x0 - 0x7F)
void sendMidiModulation(void)
{
	int rc;
	uint8_t status_byte = 0xB0 | midi_tx->channel;
	uint8_t bytes[3] = {status_byte, midi_tx->controller, midi_tx->modulation};

	rc = XUartNs550_Send(&UART_inst, bytes, 3);		// send through UART

	return;
}

// set modulation in MIDI receiver struct
void setModulationRX(uint8_t modulation)
{
	midi_rx->modulation = modulation;

	return;
}

// get modulation value from MIDI receiver struct
uint8_t getModulationRX(void)
{
	return midi_rx->modulation;
}

// set the number of currently active voices in the MIDI receiver struct
static void setActive(void)
{
	midi_rx->numActive = 0;
	for (int index = 0; index < NUM_INST; index++)
	{
		if (midi_rx->notesOn[index] > 0)	// ignore notes with MIDI note number zero
		{
			(midi_rx->numActive++);
		}
	}

	return;
}

// Read the note data register in the MIDI processor and write the currently active notes to the MIDI receiver struct
void setNotes(void)
{
	uint32_t notes_lo = MIDI_processor_getNotesLow();
	uint32_t notes_hi = MIDI_processor_getNotesHigh();
	for (int index = 0; index < NUM_INST / 2; index++)
	{
		midi_rx->notesOn[index  ] = ((notes_lo & (0xFF << (8*index))) >> (8*index));	// we have to reverse the order--first
		midi_rx->notesOn[index+4] = ((notes_hi & (0xFF << (8*index))) >> (8*index));	// note appears as MSB, last as LSB
	}
	setActive();	// update number of active voices
	return;
}

