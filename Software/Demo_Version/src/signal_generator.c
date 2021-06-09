// Author: Mel Murphy

#include "signal_generator.h"

/************************* Variable Declarations ****************************/

static signal_generator_t* sigGen;				// Signal generator struct

/************************ Implementation Functions **************************/

// Initialize signal generator
void initializeSigGen(signal_generator_t* sigGen_p)
{
	xil_printf("\n\nInitializing Signal Generator...\r\n");

	sigGen = sigGen_p;

	for (int index = 0; index < NUM_POLY; index++)
	{
		sigGen->velocity[index] 	= AMP_MAX / 2;
		sigGen->frequency[index] 	= 0;
		sigGen->cycle[index] 		= 0;
		sigGen->high_cycle[index] 	= 0;
		sigGen->low_cycle[index] 	= 0;
		sigGen->riseCycle[index] 	= RC_MAX / 2;
		sigGen->rampUp[index] 		= 0;
		sigGen->rampDown[index] 	= 0;
		sigGen->peak[index] 		= 0;
		sigGen->c[index] 			= 0;
	}

	sigGen->calcIndex = 0;
	sigGen->readIndex = BUFFER_SIZE - 1;

	return;
}

// generate pulse wave samples
uint32_t pulseWave(uint8_t signalIndex)
{
	uint32_t sample;	// calculated sample value

	// calculate constants only on the beginning of a cycle t save processing power
	if (sigGen->c[signalIndex] >= sigGen->cycle[signalIndex])	// reset cycle counter when at the end of a wave cycle
	{
		sigGen->c[signalIndex] = 0;
	}

	if (sigGen->frequency[signalIndex] == 0)		// output nothing if frequency is zero
	{
		sample = 0x800000;
	}
	else if (sigGen->c[signalIndex] < sigGen->high_cycle[signalIndex])
	{
		sample = sigGen->peak[signalIndex];	// high cycle
	}
	else
	{
		sample = 0x800000;		// low cycle
	}

	(sigGen->c[signalIndex])++;		// increment cycle counter

	return sample;

}

uint32_t sawtoothWave(uint8_t signalIndex)
{
	uint32_t sample;	// calculated sample value

	float sampleVal;

	// calculate constants only on the beginning of a cycle t save processing power
	if (sigGen->c[signalIndex] > sigGen->cycle[signalIndex])	// recalculate constants at beginning of cycle
	{
		sigGen->c[signalIndex] = 0;
	}

		// sawtooth values are linearly scaled as the cycle progresses
	sampleVal = sigGen->c[signalIndex] *  sigGen->rampUp[signalIndex];

	if (sigGen->frequency[signalIndex] == 0)		// output nothing if frequency is zero
	{
		sample = 0x800000;
	}
	else
	{
		sample = (uint32_t)sampleVal;
	}

	(sigGen->c[signalIndex])++;		// increment cycle counter

	return sample;
}
uint32_t sawTriRampWave(uint8_t signalIndex)
{
	uint32_t sample;	// calculated sample value

	float sampleVal;

	if (sigGen->c[signalIndex] > sigGen->cycle[signalIndex])	// recalculate constants at beginning of cycle
	{
		sigGen->c[signalIndex] = 0;
	}

		// rising side of triangle wave
	if (sigGen->c[signalIndex] <= sigGen->high_cycle[signalIndex])
	{
		sampleVal = sigGen->c[signalIndex] * sigGen->rampUp[signalIndex];
	}
		// falling side of triangle wave
	else
	{
		sampleVal = sigGen->peak[signalIndex] - (sigGen->c[signalIndex] * sigGen->rampDown[signalIndex]);
	}

	if (sigGen->frequency[signalIndex] == 0)		// output nothing if frequency is zero
	{
		sample = 0x800000;
	}
	else
	{
		sample = (uint32_t)sampleVal;
	}

	(sigGen->c[signalIndex])++;		// increment cycle counter

	return sample;

}

// combine multiple samples
uint32_t mixer(uint8_t numSignals, uint32_t signalArray[])
{
	uint32_t mixedSignal;
	uint32_t signalSum = 0;

	for (int index = 0; index < numSignals; index++)
	{
		signalSum += signalArray[index];		// sum the signals
	}

	mixedSignal = signalSum / numSignals;		// divide to get the average
	return mixedSignal;
}

// increment index of sample
uint32_t incrementSample(void)
{
	(sigGen->readIndex)++;
	if (sigGen->readIndex > (BUFFER_SIZE - 1))	// wrap around if past the end of the buffer
	{
		sigGen->readIndex = 0;
	}

	return sigGen->readIndex;
}

void setPitch(uint8_t index, uint8_t pitch)
{
	if (pitch > 0)
	{
		sigGen->frequency[index] = note_to_freq(pitch);
	}
	else
	{
		sigGen->frequency[index] = 0;
	}
}

float getFrequency(uint8_t index)
{
	return sigGen->frequency[index];
}

void clearPitch(uint8_t index)
{
	sigGen->frequency[index] = 0;
}

// send next sample to I2S
void sendSample(void)
{
	uint32_t sample = XBram_ReadReg(BUFFER_BRAM_BASEADDR, sigGen->readIndex);
	I2S2_Send_Sample(sample);
	return;
}

void bufferSample(uint32_t sample, uint32_t index)
{
	XBram_WriteReg(BUFFER_BRAM_BASEADDR, index, sample);
	return;
}


