// Author: Mel Murphy

#include "signal_generator.h"

/************************* Variable Declarations ****************************/

static signal_generator_t* sigGen;				// Signal generator struct

/************************ Implementation Functions **************************/


void initializeNote(volatile note_t* note)
{
	note->velocity	= AMP_MAX;
	note->frequency = NOTE_A4;
	note->duty_cycle = DC_MAX / 2;
	note->note_on = true;

}

void initializeSigGen(signal_generator_t* sigGen_p)
{
	xil_printf("\n\nInitializing Signal Generator...\r\n");

	sigGen = sigGen_p;

	for (int index = 0; index < NUM_POLY; index++)
	{
		sigGen->cycle[index] = 0;
		sigGen->high_cycle[index] = 0;
		sigGen->low_cycle[index] = 0;
		sigGen->rampUp[index] = 0;
		sigGen->rampDown[index] = 0;
		sigGen->peak[index] = 0;
		sigGen->c[index] = 0;
	}

	sigGen->calcIndex = 0;
	sigGen->readIndex = BUFFER_SIZE - 1;

	return;
}

uint32_t pulseWave(float frequency, uint8_t amplitude, uint8_t dutyCycle, uint8_t signalIndex)
{
	uint32_t sample;	// calculated sample value

	if (sigGen->c[signalIndex] > sigGen->cycle[signalIndex])	// reset cycle counter when at the end of a wave cycle
	{
		sigGen->peak[signalIndex] = (amplitude * ((1 << BIT_DEPTH) - 1)) / AMP_MAX;
		sigGen->cycle[signalIndex] = (SAMPLE_RATE << 1) / frequency;		// length of full cycle
		sigGen->high_cycle[signalIndex] = (dutyCycle * sigGen->cycle[signalIndex]) / DC_MAX;	// length of high part of duty cycle
		sigGen->c[signalIndex] = 0;
	}

	if (sigGen->c[signalIndex] <= sigGen->high_cycle[signalIndex])
	{
		sample = sigGen->peak[signalIndex];	// high cycle
	}
	else
	{
		sample = 0x800000;		// low cycle
	}

	(sigGen->c[signalIndex])++;


	return sample;

}

uint32_t sawtoothWave(float frequency, uint8_t amplitude, uint8_t signalIndex, uint16_t sampleIndex)
{
	uint32_t sample;	// calculated sample value

	float sampleVal;

	sampleVal = sampleIndex *  sigGen->rampUp[signalIndex];
	sample = (uint32_t)sampleVal;

	if (sampleIndex == 0)	// recalculate constants at beginning of cycle
	{
		sigGen->cycle[signalIndex] = (SAMPLE_RATE << 1 ) / frequency;
		sigGen->rampUp[signalIndex] = sigGen->peak[signalIndex] / sigGen->cycle[signalIndex];
		sigGen->peak[signalIndex] = (amplitude * ((1 << BIT_DEPTH) - 1)) / AMP_MAX;
	}

	return sample;
}
uint32_t sawTriRampWave(uint16_t frequency, uint8_t amplitude, uint8_t riseCycle, uint8_t signalIndex, uint16_t sampleIndex)
{
	uint32_t sample;	// calculated sample value

	float sampleVal;

	if (sampleIndex <= sigGen->high_cycle[signalIndex])
	{
		sampleVal = sampleIndex * sigGen->rampUp[signalIndex];
	}
	else
	{
		sampleVal = sigGen->peak[signalIndex] - (sampleIndex * sigGen->rampDown[signalIndex]);
	}
	sample = (uint32_t)sampleVal;

	if (sampleIndex == 0)	// reset cycle counter when at the end of a wave cycle
	{
		sigGen->peak[signalIndex] = (amplitude * ((1 << BIT_DEPTH) - 1)) / AMP_MAX;
		sigGen->cycle[signalIndex] = (SAMPLE_RATE << 1 ) / frequency;
		sigGen->high_cycle[signalIndex] = (riseCycle * sigGen->cycle[signalIndex]) / RC_MAX;	// length of high part of duty cycle
		sigGen->low_cycle[signalIndex] = sigGen->cycle[signalIndex] - sigGen->high_cycle[signalIndex];
		sigGen->rampUp[signalIndex] = sigGen->peak[signalIndex] / sigGen->high_cycle[signalIndex];
		sigGen->rampDown[signalIndex] = sigGen->peak[signalIndex] / sigGen->low_cycle[signalIndex];
	}

	return sample;

}

uint32_t mixer(uint8_t numSignals, uint32_t signalArray[])
{
	uint32_t mixedSignal;
	uint32_t signalSum = 0;

	for (int index = 0; index < numSignals; index++)
	{
		signalSum += signalArray[index];
	}

	mixedSignal = signalSum / numSignals;

	return mixedSignal;
}

// increment index of sample
uint32_t incrementSample(void)
{
	(sigGen->readIndex)++;
	if (sigGen->readIndex > (BUFFER_SIZE - 1))
	{
		sigGen->readIndex = 0;
	}

	return sigGen->readIndex;

}

void play_note(void)
{
	return;
}

void sendSample(void)
{
	I2S2_Send_Sample(sigGen->sampleBuffer[sigGen->readIndex]);

	return;
}
