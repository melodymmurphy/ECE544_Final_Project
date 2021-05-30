// Author: Mel Murphy

#ifndef _SIGNAL_GENERATOR_H
#define _SIGNAL_GENERATOR_H

/****************************** Includes *********************************/
#include "main.h"

/****************************** Definitions ******************************/

#define SAMPLE_RATE 	48000
#define BIT_DEPTH		24
#define MAX_AMP 		((1 << BIT_DEPTH) - 1)
#define DC_MAX			255
#define RC_MAX			255
#define AMP_MAX			255
#define NOTE_A4			440
#define MAX_SAW  		3
#define MAX_PULSE		4
#define NUM_POLY		4
#define BUFFER_SIZE		2048

/*************************** Type Definitions ****************************/

typedef struct
{
	float cycle[NUM_POLY];
	float high_cycle[MAX_PULSE];
	float low_cycle[MAX_PULSE];
	float rampUp[NUM_POLY];
	float rampDown[NUM_POLY];
	uint32_t peak[MAX_SAW];
	volatile uint32_t c[NUM_POLY];			// sample counter
	uint16_t calcIndex;
	volatile uint16_t readIndex;
	uint32_t sampleBuffer[BUFFER_SIZE];
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
uint32_t sawtoothWave(float frequency, uint8_t amplitude, uint8_t signalIndex, uint16_t sampleIndex);
uint32_t sawTriRampWave(uint16_t frequency, uint8_t amplitude, uint8_t riseCycle, uint8_t signalIndex, uint16_t sampleIndex);
uint32_t mixer(uint8_t numSignals, uint32_t signalArray[]);
uint32_t incrementSample(void);
void sendSample(void);

#endif
