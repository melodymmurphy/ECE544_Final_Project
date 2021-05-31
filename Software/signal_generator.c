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
	sigGen = sigGen_p;

	for (int index = 0; index < NUM_POLY; index++)
	{
		sigGen->c[index] = 0;
	}

	sigGen->nextSample = 0;

	return;
}

uint32_t pulseWave(float frequency, uint8_t amplitude, uint8_t dutyCycle, uint8_t signalIndex)
{
	uint32_t sample;	// calculated sample value

	static float cycle[MAX_PULSE];
	static uint32_t high_cycle[MAX_PULSE];
	static uint32_t peak[MAX_PULSE];

	if (sigGen->c[signalIndex] <= high_cycle[signalIndex])
	{
		sample = peak[signalIndex];	// high cycle
	}
	else
	{
		sample = 0x800000;		// low cycle
	}

	if (sigGen->c[signalIndex] >= cycle[signalIndex])	// reset cycle counter when at the end of a wave cycle
	{
		peak[signalIndex] = (amplitude * ((1 << BIT_DEPTH) - 1)) / AMP_MAX;
		cycle[signalIndex] = (SAMPLE_RATE << 1) / frequency;		// length of full cycle
		high_cycle[signalIndex] = (dutyCycle * cycle[signalIndex]) / DC_MAX;	// length of high part of duty cycle
		sigGen->c[signalIndex] = 0;
	}

	return sample;

}

uint32_t sawtoothWave(float frequency, uint8_t amplitude, uint8_t signalIndex)
{
	uint32_t sample;	// calculated sample value

	static float ramp[MAX_SAW];
	static float cycle[MAX_SAW];
	static uint32_t peak[MAX_SAW];
	float sampleVal;

	sampleVal = sigGen->c[signalIndex] *  ramp[signalIndex];
	sample = (uint32_t)sampleVal;

	if (sigGen->c[signalIndex] >= cycle[signalIndex])	// reset cycle counter when at the end of a wave cycle
	{
		cycle[signalIndex] = (SAMPLE_RATE << 1 ) / frequency;
		ramp[signalIndex] = peak[signalIndex] / cycle[signalIndex];
		peak[signalIndex] = (amplitude * ((1 << BIT_DEPTH) - 1)) / AMP_MAX;
		sigGen->c[signalIndex] = 0;
	}

	return sample;
}
uint32_t sawTriRampWave(uint16_t frequency, uint8_t amplitude, uint8_t riseCycle, uint8_t signalIndex)
{
	uint32_t sample;	// calculated sample value

	static float cycle[MAX_SAW];
	static float upCycle[MAX_SAW];
	static float downCycle[MAX_SAW];
	static float upRamp[MAX_SAW];
	static float downRamp[MAX_SAW];
	static uint32_t peak[MAX_SAW];
	float sampleVal;

	if (sigGen->c[signalIndex] <= upCycle[signalIndex])
	{
		sampleVal = sigGen->c[signalIndex] * upRamp[signalIndex];
	}
	else
	{
		sampleVal = peak[signalIndex] - (sigGen->c[signalIndex] * downRamp[signalIndex]);
	}
	sample = (uint32_t)sampleVal;

	if (sigGen->c[signalIndex] >= cycle[signalIndex])	// reset cycle counter when at the end of a wave cycle
	{
		peak[signalIndex] = (amplitude * ((1 << BIT_DEPTH) - 1)) / AMP_MAX;
		cycle[signalIndex] = (SAMPLE_RATE << 1 ) / frequency;
		upCycle[signalIndex] = (riseCycle * cycle[signalIndex]) / RC_MAX;	// length of high part of duty cycle
		downCycle[signalIndex] = cycle[signalIndex] - upCycle[signalIndex];
		upRamp[signalIndex] = peak[signalIndex] / upCycle[signalIndex];
		downRamp[signalIndex] = peak[signalIndex] / downCycle[signalIndex];
		sigGen->c[signalIndex] = 0;
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

// increment index of each sample
void incrementSamples(void)
{

	(sigGen->c[0])++;
	(sigGen->c[1])++;
	(sigGen->c[2])++;
	(sigGen->c[3])++;

}

void play_note(void)
{
	return;
}

void sendSample(void)
{
	I2S2_Send_Sample(sigGen->nextSample);

	return;
}
