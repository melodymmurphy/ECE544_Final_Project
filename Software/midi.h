// Author: Mel Murphy

#ifndef _MIDI_H
#define _MIDI_H

/****************************** Includes *********************************/

#include "main.h"
#include "signal_generator.h"

/***************************** Definitions *******************************/

#define NUM_INST		NUM_POLY		// number of available active instruments
#define PITCH_A4		69
#define CHANNEL_0		0

/*************************** Type Definitions ****************************/

typedef struct
{
	uint8_t active;
	uint8_t numActive;				// number of currently active notes
	uint8_t notesOn[NUM_INST + 1];	// which notes are currently active
	uint8_t velocity[NUM_INST + 1];		// velocity of active notes
	uint8_t modulation;				// modulation value

} midi_rx_t;

typedef struct
{
	uint8_t channel;		// MIDI channel
	uint8_t pitch;			// which notes are currently active
	uint8_t velocity;		// velocity of active notes
	uint8_t controller;		// MIDI controller number
	uint8_t modulation;		// modulation value

} midi_tx_t;


/************************* Function Prototypes ***************************/

void initialize_MIDI(midi_rx_t* midi_rx_p, midi_tx_t* midi_tx_p);
void setMidiOutNote(uint8_t channel, uint8_t pitch, uint8_t velocity);
void setMidiOutMod(uint8_t channel, uint8_t controller, uint8_t modulation);
void sendMidiNoteOn(void);
void sendMidiNoteOff(void);
void sendMidiModulation(void);


#endif
