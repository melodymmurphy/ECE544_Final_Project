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
#include "system.h"
#include "initialization.h"
#include "interrupts.h"
#include "display.h"
#include "sequencer.h"
#include "menu.h"
#include "signal_generator.h"
#include "control.h"
#include "midi.h"
#include "menu.h"
#include "luts.h"

/************************** Variable Declarations ****************************/

menu_main 			menu_inst;
system_t			system_inst;


/**************************** Function Prototypes ****************************/

static void calculate_samples(void* pvParameters);
static void sequence_thread(void* pvParameters);
static void menu_thread(void* pvParameters);
static void midi_thread(void* pvParameters);
static void calculate_constants(void* pvParameters);

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
	vSemaphoreCreateBinary(timer_sem);
	vSemaphoreCreateBinary(display_sem);
	vSemaphoreCreateBinary(note_sem);
	vSemaphoreCreateBinary(mod_sem);
	vSemaphoreCreateBinary(step_sem);
	vSemaphoreCreateBinary(bypass_sem);
	vSemaphoreCreateBinary(calc_sem);

	// initialize software instances
	initialize_system(&system_inst);
	menu_init(&menu_inst);

	I2S2_Send_Sample(0);	// Send one sample to I2S output to trigger the ready interrupts


	// Create tasks
	xTaskCreate(calculate_samples,
				"Audio Sample Generation",
				512,
				NULL,
				2,
				NULL);

	xTaskCreate(sequence_thread,
				"Sequencer",
				256,
				NULL,
				4,
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
				3,
				NULL);

	xTaskCreate(calculate_constants,
				"Calculate Sample Constants",
				256,
				NULL,
				4,
				NULL);


	//Start the Scheduler
	xil_printf("Starting the scheduler\r\n");
	vTaskStartScheduler();

	return -1;
}

void calculate_constants(void* pvParameters)
{

	while(1)
	{
		if (xSemaphoreTake(calc_sem, minimum_wait))
		{
			compute_constants(0);
			compute_constants(1);
			compute_constants(2);
		}

		vTaskDelay(1);
	}

	vTaskDelete(NULL);
}

// Fill audio buffer with samples to send to I2S TX module
void calculate_samples(void* pvParameters)
{
	uint32_t samples[NUM_POLY];
	float freq[NUM_POLY];
	uint8_t modulation;
	uint32_t mixedSample;
	uint8_t numActive;
	mode_t mode;

	while(1)
	{
		mode = getMode();

		if (xSemaphoreTake(I2S_TX_low_sem, minimum_wait))
		{
			for (int index = 0; index < (BUFFER_SIZE >> 1); index++)
			{
				samples[0] = pulseWave(0);
				samples[1] = pulseWave(1);
				samples[2] = pulseWave(2);
				mixedSample = samples[0] + samples[1] + samples[2];
				bufferSample(mixedSample, index);
			}
		}
		if (xSemaphoreTake(I2S_TX_high_sem, minimum_wait))
		{
			for (int index = (BUFFER_SIZE >> 1); index < BUFFER_SIZE; index++)
			{
					samples[0] = pulseWave(0);
					samples[1] = pulseWave(1);
					samples[2] = pulseWave(2);
					mixedSample = samples[0] + samples[1] + samples[2];
					bufferSample(mixedSample, index);
			}
		}
	}

	vTaskDelete(NULL);
}

void sequence_thread(void* pvParameters)
{
	mode_t mode;
	uint8_t btns;
	uint8_t encSw;

	while(1)
	{
		mode = getMode();
		if (xSemaphoreTake(timer_sem, minimum_wait))
		{
			if (mode == PLAY)
			{
				stop_note();
				quiet_note(0);
				next_step();
				seq_timer();
				play_note();
				xSemaphoreGive(calc_sem);
				vTaskDelay(1);
				send_note();
				updateLEDs();
			}
			else if (mode == RECORD)
			{
				updateLEDs();
				toggleBlink();
			}
			else if (mode == BYPASS)
			{
				updateLEDs();
				toggleBlink();
			}
			if (mode == MEMORY)
			{
				stop_note();
				quiet_note(0);
				seq_timer();
				play_note();
				xSemaphoreGive(calc_sem);
				vTaskDelay(1);
				send_note();
				updateLEDs();
			}
		}
		if (xSemaphoreTake(step_sem, minimum_wait))
		{
			btns = getButtons();
			if (mode == PLAY)
			{
				if (btns & BTNC)
				{
					setMode(RECORD);
				}
			}
			else if (mode == RECORD)
			{
				if (btns & BTNC)
				{
					setMode(PLAY);
				}
				else if (btns & BTNR)
				{
					stepSeqForward();
				}
				else if (btns & BTNL)
				{
					stepSeqBackward();
				}
			}
		}
		if (xSemaphoreTake(bypass_sem, minimum_wait))
		{
			encSw = (uint8_t)PMODENC544_getBtnSwReg() & ENC_SW_MASK;
			if (mode == PLAY || mode == RECORD || mode == MEMORY)
			{
				if (!encSw)
				{
					setMode(BYPASS);
				}
			}
			else if (mode == BYPASS)
			{
				if (encSw)
				{
					setMode(RECORD);
				}
			}
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

		vTaskDelay(1);
	}

	vTaskDelete(NULL);
}

static void midi_thread(void* pvParameters)
{
	uint8_t mod;
	mode_t mode;
	uint32_t notes_hi;
	while(1)
	{
		mode = getMode();

		if (xSemaphoreTake(note_sem, minimum_wait))
		{
			if (mode == BYPASS || mode == MEMORY || mode == PLAY)
			{
				setNotes();
				getFreqs();
				xSemaphoreGive(calc_sem);
			}
			if (mode == RECORD)
			{
				notes_hi = MIDI_processor_getNotesHigh();
				if (((notes_hi & NOTE_0_MASK) >> 24) > getNote(7))
				{
					setNotePitch((notes_hi & NOTE_0_MASK) >> 24);
					stepSeqForward();
				}
				setNotes();
				getFreqs();
				xSemaphoreGive(calc_sem);
			}
		}
		if (xSemaphoreTake(mod_sem, minimum_wait))
		{
			mod = MIDI_processor_getModulation();
			setModulationRX(mod);
			setMod(0, mod);
			setMod(1, mod);
			setMod(2, mod);
			xSemaphoreGive(calc_sem);
		}

		vTaskDelay(1);
	}

	vTaskDelete(NULL);
}

