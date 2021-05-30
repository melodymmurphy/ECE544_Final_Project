#ifndef _LUTS_H
#define _LUTS_H

/****************************** Includes *********************************/

#include "main.h"
#include "midi.h"

/****************************** Definitions **********************************/

#define NOTES		128

/***************************** Lookup Tables *********************************/

// convert a MIDI note to its corresponding frequency

float note_to_freq(uint8_t note);
char* note_to_str(uint8_t note);

#endif
