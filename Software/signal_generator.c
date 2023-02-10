/**
*
* @file signal_generator.c
*
* @author Mel Murphy (cumurphy@pdx.edu)
*
* Audio signal generation
*
******************************************************************************/

/****************************** Includes *********************************/

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


	if (sigGen->c[signalIndex] >= sigGen->cycle[signalIndex])	// reset cycle counter when at the end of a wave cycle
	{
		sigGen->c[signalIndex] = 0;
	}

	if (sigGen->frequency[signalIndex] == 0)		// output if frequency is zero
	{
		sample = 0x800000;
	}
	else if (sigGen->c[signalIndex] < sigGen->high_cycle[signalIndex])	// if cycle counter is less than high cycle value
	{
		sample = sigGen->peak[signalIndex];	// high cycle
	}
	else	// if cycle counter is greater than high cycle value
	{
		sample = 0x800000;		// low cycle
	}

	(sigGen->c[signalIndex])++;		// increment cycle counter

	return sample;

}

// generate sawtooth wave samples
uint32_t sawtoothWave(uint8_t signalIndex)
{
	uint32_t sample;	// calculated sample value

	float sampleVal;

	if (sigGen->c[signalIndex] > sigGen->cycle[signalIndex])	// reset cycle counter when at the end of a wave cycle
	{
		sigGen->c[signalIndex] = 0;
	}


	sampleVal = sigGen->c[signalIndex] *  sigGen->rampUp[signalIndex];	// sawtooth values are linearly scaled as the cycle progresses

	if (sigGen->frequency[signalIndex] == 0)		// output if frequency is zero
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

	if (sigGen->c[signalIndex] > sigGen->cycle[signalIndex])
	{
		sigGen->c[signalIndex] = 0;
	}


	if (sigGen->c[signalIndex] <= sigGen->high_cycle[signalIndex])	// rising side of triangle wave
	{
		sampleVal = sigGen->c[signalIndex] * sigGen->rampUp[signalIndex];	// reset cycle counter when at the end of a wave cycle
	}

	else	// falling side of triangle wave
	{
		sampleVal = sigGen->peak[signalIndex] - (sigGen->c[signalIndex] * sigGen->rampDown[signalIndex]);
	}

	if (sigGen->frequency[signalIndex] == 0)		// output if frequency is zero
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

// combine multiple samples	(deprecated--integer division is too slow to be run at every sample)
uint32_t mixer(uint8_t numSignals, uint32_t signalArray[])
{
	uint32_t mixedSignal;
	uint32_t signalSum = 0;

	for (int index = 0; index < numSignals; index++)	// for every active signal
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

// set the frequency in the signal generator of the desired voice based on the note number
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

// get the frequency of the selected voice
float getFrequency(uint8_t index)
{
	return sigGen->frequency[index];
}

// clear the frequency of the selected voice (effectively a note off message in our program)
void clearPitch(uint8_t index)
{
	sigGen->frequency[index] = 0;
}

// send next sample to I2S
void sendSample(void)
{
	uint32_t sample = XBram_ReadReg(BUFFER_BRAM_BASEADDR, sigGen->readIndex);	// read sample from buffer in BRAM
	I2S2_Send_Sample(sample);	// send sample to I2S2 module
	return;
}

// write sample to buffer
void bufferSample(uint32_t sample, uint32_t index)
{
	XBram_WriteReg(BUFFER_BRAM_BASEADDR, index, sample);	// write sample to buffer in BRAM
	return;
}


