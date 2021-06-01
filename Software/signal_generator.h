// Author: Mel Murphy

#ifndef _SIGNAL_GENERATOR_H
#define _SIGNAL_GENERATOR_H

/****************************** Includes *********************************/
#include "main.h"

/****************************** Definitions ******************************/

#define SAMPLE_RATE 	48000
#define BIT_DEPTH		24
#define MAX_AMP 		((1 << BIT_DEPTH) - 1)
#define DC_MAX			127
#define RC_MAX			127
#define AMP_MAX			127
#define NOTE_A4			440		// frequency value of MIDI note A4
#define MAX_SAW  		3
#define MAX_PULSE		4
#define NUM_POLY		4
#define BUFFER_SIZE		8192

/*************************** Type Definitions ****************************/

typedef struct
{
	uint8_t velocity[NUM_POLY];
	float frequency[NUM_POLY];
	float cycle[NUM_POLY];
	float high_cycle[NUM_POLY];
	float low_cycle[NUM_POLY];
	float rampUp[NUM_POLY];
	float rampDown[NUM_POLY];
	uint32_t peak[NUM_POLY];
	volatile uint32_t c[NUM_POLY];			// sample counter
	uint16_t calcIndex;
	volatile uint16_t readIndex;
	uint32_t sampleBuffer[BUFFER_SIZE];
} signal_generator_t;


/************************* Function Prototypes ***************************/

void initializeSigGen(signal_generator_t* sigGen_p);
uint32_t pulseWave(float frequency, uint8_t amplitude, uint8_t dutyCycle, uint8_t signalIndex);
uint32_t sawtoothWave(float frequency, uint8_t amplitude, uint8_t signalIndex);
uint32_t sawTriRampWave(uint16_t frequency, uint8_t amplitude, uint8_t riseCycle, uint8_t signalIndex);
uint32_t mixer(uint8_t numSignals, uint32_t signalArray[]);
uint32_t incrementSample(void);
void sendSample(void);

#endif
