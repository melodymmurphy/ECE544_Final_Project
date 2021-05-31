// Author: Mel Murphy

#ifndef _SIGNAL_GENERATOR_H
#define _SIGNAL_GENERATOR_H

/****************************** Includes *********************************/
#include "main.h"

/****************************** Definitions ******************************/

#define SAMPLE_RATE 48000
#define BIT_DEPTH	24
#define MAX_AMP 	((1 << BIT_DEPTH) - 1)
#define DC_MAX		255
#define RC_MAX		255
#define AMP_MAX		255
#define NOTE_A4		440
#define MAX_SAW  	3
#define MAX_PULSE	4
#define NUM_POLY	4

/*************************** Type Definitions ****************************/

typedef struct
{
	volatile uint32_t c[NUM_POLY];	// sample counter
	uint32_t nextSample;
} signal_generator_t;

typedef struct
{
	uint8_t velocity;
	float frequency;
	uint8_t duty_cycle;
	uint8_t rise_cycle;
	bool note_on;

} note_t;


/************************* Function Prototypes ***************************/

void initializeNote(volatile note_t* note);
void initializeSigGen(signal_generator_t* sigGen_p);
uint32_t pulseWave(float frequency, uint8_t amplitude, uint8_t dutyCycle, uint8_t signalIndex);
uint32_t sawtoothWave(float frequency, uint8_t amplitude, uint8_t signalIndex);
uint32_t sawTriRampWave(uint16_t frequency, uint8_t amplitude, uint8_t riseCycle, uint8_t signalIndex);
uint32_t mixer(uint8_t numSignals, uint32_t signalArray[]);
void incrementSamples(void);
void sendSample(void);

#endif
