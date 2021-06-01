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
#include "midi.h"
#include "menu.h"
#include "luts.h"

/************************** Variable Declarations ****************************/

sequencer_t		 	sequencer_inst;
signal_generator_t 	sigGen_inst;
midi_rx_t			midi_rx_inst;
midi_tx_t			midi_tx_inst;
menu_main 			menu_inst;


/**************************** Function Prototypes ****************************/

static void calculate_samples(void* pvParameters);
static void sequence_thread(void* pvParameters);
static void menu_thread(void* pvParameters);
static void midi_thread(void* pvParameters);

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
	vSemaphoreCreateBinary(note_on_sem);
	vSemaphoreCreateBinary(note_off_sem);
	vSemaphoreCreateBinary(mod_sem);

	// initialize software instances
	initialize_sequencer(&sequencer_inst);
	initializeSigGen(&sigGen_inst);
	initialize_MIDI(&midi_rx_inst, &midi_tx_inst);
	menu_init(&menu_inst);

	I2S2_Send_Sample(0);	// Send one sample to I2S output to trigger the ready interrupts


	// Create tasks
	xTaskCreate(calculate_samples,
				"Audio Sample Generation",
				1024,
				NULL,
				2,
				NULL);

	xTaskCreate(sequence_thread,
				"Sequencer",
				256,
				NULL,
				2,
				NULL);

	xTaskCreate(menu_thread,
				"OLED menu",
				256,
				NULL,
				1,
				NULL);

	xTaskCreate(midi_thread,
				"MIDI receive",
				256,
				NULL,
				2,
				NULL);




	//Start the Scheduler
	xil_printf("Starting the scheduler\r\n");
	vTaskStartScheduler();

/*	// cleanup and exit
	shutdown_system();

*/

	return -1;
}

// Fill audio buffer with samples to send to I2S TX module
void calculate_samples(void* pvParameters)
{
	uint32_t samples[NUM_POLY];
	float freq[NUM_POLY];
	uint8_t activeNotes = 1;
	uint8_t modulation;

	while(1)
	{
		modulation = midi_rx_inst.modulation;
		freq[0] = sigGen_inst.frequency[0];
		if (xSemaphoreTake(I2S_TX_low_sem, minimum_wait))
		{
			for (int index = 0; index < (BUFFER_SIZE >> 1); index++)
			{
				samples[0] = sawTriRampWave(freq[0], 127, modulation, 0);
				sigGen_inst.sampleBuffer[index] = mixer(activeNotes, samples);
			}
		}
		else if (xSemaphoreTake(I2S_TX_high_sem, minimum_wait))
		{
			for (int index = (BUFFER_SIZE >> 1); index < BUFFER_SIZE; index++)
			{
				samples[0] = sawTriRampWave(freq[0], 127, modulation, 0);
				sigGen_inst.sampleBuffer[index] = mixer(activeNotes, samples);
			}
		}
	}

	vTaskDelete(NULL);
}

void sequence_thread(void* pvParameters)
{
	while(1)
	{
		if (xSemaphoreTake(step_sem, minimum_wait))
		{
			next_step();
			play_note(&sigGen_inst);
			stop_note();
			vTaskDelay(1);
			send_note();
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
		if (xSemaphoreTake(display_sem, minimum_wait))
		{
			menu_nav();
		}

		vTaskDelay(20);
	}

	vTaskDelete(NULL);
}

static void midi_thread(void* pvParameters)
{
	uint8_t midiByte;
	while(1)
	{
		if (xSemaphoreTake(note_on_sem, minimum_wait))
		{
			midiByte = MIDI_processor_getNote();
			sigGen_inst.frequency[0] = note_to_freq(midiByte);
		}
		else if (xSemaphoreTake(note_off_sem, minimum_wait))
		{
			midiByte = MIDI_processor_getNote();
			sigGen_inst.frequency[0] = 0;
		}
		else if (xSemaphoreTake(mod_sem, minimum_wait))
		{
			midiByte = MIDI_processor_getModulation();
			midi_rx_inst.modulation = midiByte;
		}

		vTaskDelay(1);
	}

	vTaskDelete(NULL);
}

