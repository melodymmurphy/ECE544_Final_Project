// Author: Mel Murphy

#ifndef _SIGNAL_GENERATOR_H
#define _SIGNAL_GENERATOR_H

/****************************** Includes *********************************/
#include "main.h"
#include "luts.h"

/****************************** Definitions ******************************/

#define SAMPLE_RATE 	48000
#define BIT_DEPTH		24
#define MAX_AMP 		((1 << BIT_DEPTH) - 1)
#define DC_MIN			0
#define DC_MAX			127
#define RC_MIN			0
#define RC_MAX			127
#define AMP_MIN			0
#define AMP_MAX			127
#define NOTE_A4			440		// frequency value of MIDI note A4
#define NUM_POLY		8
#define BUFFER_SIZE		8192

/*************************** Type Definitions ****************************/

typedef struct
{
	uint8_t velocity[NUM_POLY];
	float frequency[NUM_POLY];
	float cycle[NUM_POLY];
	float high_cycle[NUM_POLY];
	float low_cycle[NUM_POLY];
	float riseCycle[NUM_POLY];
	float rampUp[NUM_POLY];
	float rampDown[NUM_POLY];
	uint32_t peak[NUM_POLY];
	volatile uint32_t c[NUM_POLY];			// sample counter
	uint16_t calcIndex;
	volatile uint16_t readIndex;
//	uint32_t sample_buffer[BUFFER_SIZE];
} signal_generator_t;


/************************* Function Prototypes ***************************/

void initializeSigGen(signal_generator_t* sigGen_p);
uint32_t pulseWave(uint8_t signalIndex);
uint32_t sawtoothWave(uint8_t signalIndex);
uint32_t sawTriRampWave(uint8_t signalIndex);
uint32_t mixer(uint8_t numSignals, uint32_t signalArray[]);
uint32_t incrementSample(void);
void setPitch(uint8_t index, uint8_t pitch);
float getFrequency(uint8_t index);
void clearPitch(uint8_t index);
void sendSample(void);
void bufferSample(uint32_t sample, uint32_t index);

#endif
