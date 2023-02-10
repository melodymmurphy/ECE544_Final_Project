/**
*
* @file luts.h
*
* @author Mel Murphy (cumurphy@pdx.edu)
*
* Header file for MIDI note to frequency or note name conversions
*
******************************************************************************/

#ifndef _LUTS_H
#define _LUTS_H

/****************************** Includes *********************************/

#include "main.h"

/****************************** Definitions **********************************/

#define NOTES		128		// total number of MIDI notes

/***************************** Lookup Tables *********************************/

// convert a MIDI note to its corresponding frequency

float note_to_freq(uint8_t note);
char* note_to_str(uint8_t note);

#endif
