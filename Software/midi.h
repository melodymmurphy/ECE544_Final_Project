// Author: Mel Murphy

#ifndef _MIDI_H
#define _MIDI_H

/****************************** Includes *********************************/

#include "main.h"
#include "signal_generator.h"

/*************************** Type Definitions ****************************/

typedef struct
{
	uint8_t numActive;				// number of currently active notes
	uint8_t notesOn[NUM_POLY];		// which notes are currently active
	uint8_t velocity[NUM_POLY];		// velocity of active notes

} midi_t;


/************************* Function Prototypes ***************************/

void initialize_MIDI(midi_t* midi_p);
void readMIDI(void);


#endif
