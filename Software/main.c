/**
*
* @file main.c
*
* @author Mel Murphy (cumurphy@pdx.edu)
*
* This file tests the first iteration of the ECE544 Final Project hardware
*
* @note
* The driver code and test application(s) for the pmodOLDrgb and pmodENC are
* based on code provided by Digilent, Inc.
******************************************************************************/

#include "main.h"
#include "initialization.h"
#include "interrupts.h"
#include "display.h"
#include "sequencer.h"
#include "menu.h"
#include "signal_generator.h"
#include "menu.h"

/************************** Variable Declarations ****************************/

sequencer_t		 	sequencer_inst;
signal_generator_t 	sigGen_inst;
menu_main 			menu_inst;

/**************************** Function Prototypes ****************************/

static void calculate_sample(void* pvParameters);
static void sequence_thread(void* pvParameters);
static void menu_thread(void* pvParameters);

/********************************* MAIN PROGRAM ******************************/

int main(void)
{

	portBASE_TYPE xStatus;	// return status for Xilinx driver functions
	xStatus = initialize_hardware();
	if (XST_SUCCESS != xStatus)
	{
		exit(1);
	}

	xil_printf("ECE 544 Final Project build test Program\n\r");
	xil_printf("By Mel Murphy, Jonathan Law, and David Hanson. Apr-2021\n\n\r");

	vSemaphoreCreateBinary(I2S_TX_low_sem);
	vSemaphoreCreateBinary(I2S_TX_high_sem);
	vSemaphoreCreateBinary(step_sem);
	vSemaphoreCreateBinary(display_sem);

	// initialize software instances
	initialize_sequencer(&sequencer_inst);
	initializeSigGen(&sigGen_inst);
	menu_init(&menu_inst);

	I2S2_Send_Sample(0);	// Send one sample to I2S output to trigger the ready interrupts


	// Create tasks
	xTaskCreate( calculate_sample,
				"Audio Sample Generation",
				1024,
				NULL,
				2,
				NULL );

	xTaskCreate( sequence_thread,
				"Sequencer",
				256,
				NULL,
				1,
				NULL );

	xTaskCreate( menu_thread,
				"OLED menu",
				256,
				NULL,
				1,
				NULL );


	//Start the Scheduler
	xil_printf("Starting the scheduler\r\n");
	vTaskStartScheduler();

/*	// cleanup and exit
	shutdown_system();

*/

	return -1;
}

// Fill audio buffer with samples to send to I2S TX module
void calculate_sample(void* pvParameters)
{

	int index;
	uint32_t samples[NUM_POLY];

	while(1)
	{
		if (xSemaphoreTake(I2S_TX_low_sem, 1))
		{
			for (index = 0; index < (BUFFER_SIZE >> 1); index++)		// lower half of buffer
			{
				samples[0] = sawtoothWave(220, 127, 0);
				samples[1] = sawtoothWave(440, 127, 1);
//				samples[2] = sawtoothWave(880, 127, 2);
//				samples[1] = pulseWave(440, 127, 127, 1);
//				samples[2] = pulseWave(880, 127, 127, 2);
				sigGen_inst.sampleBuffer[index] = mixer(2, samples);
			}
		}
		else if (xSemaphoreTake(I2S_TX_high_sem, 1))
		{
			for (index = (BUFFER_SIZE >> 1); index < BUFFER_SIZE; index++)		// upper half of buffer
			{
				samples[0] = sawtoothWave(220, 127, 0);
				samples[1] = sawtoothWave(440, 127, 1);
//				samples[2] = sawtoothWave(880, 127, 2);
//				samples[1] = pulseWave(440, 127, 127, 1);
//				samples[2] = pulseWave(880, 127, 127, 2);
				sigGen_inst.sampleBuffer[index] = mixer(2, samples);
			}
		}
	}

	vTaskDelete(NULL);
}

void sequence_thread(void* pvParameters)
{
	while(1)
	{
		if (xSemaphoreTake(step_sem, 1))
		{
			next_step();
			updateLEDs(sequencer_inst);
		}

		vTaskDelay(10);
	}

	vTaskDelete(NULL);
}

static void menu_thread(void* pvParameters)
{
	while(1)
	{
		if (xSemaphoreTake(display_sem, 1))
		{
			menu_nav();
		}

		vTaskDelay(20);
	}

	vTaskDelete(NULL);
}

/*
// ===== circular buffer implementation =====

void calculate_sample(void* pvParameters)
{

	while(1)
	{

		if (sigGen_inst.calcIndex != sigGen_inst.readIndex)		// don't cross the write pointer over the read pointer
		{
			sigGen_inst.sampleBuffer[sigGen_inst.calcIndex] = pulseWave(220, 127, 127, 0, sigGen_inst.calcIndex);
			(sigGen_inst.calcIndex)++;		// increment the index around the circular buffer
			if (sigGen_inst.calcIndex > (BUFFER_SIZE - 1))
			{
				sigGen_inst.calcIndex = 0;
			}
		}
	}

	vTaskDelete(NULL);
}

// increment index of each sample
void incrementSample(void)
{
	uint16_t nextIndex = (sigGen->readIndex) + 1;
	if (nextIndex > (BUFFER_SIZE - 1))
	{
		nextIndex = 0;
	}

	if (nextIndex != sigGen->calcIndex)		// don't cross read pointer over the write pointer
	{
		sigGen->readIndex = nextIndex;
	}

}

*/


