/**
*
* @file main.c
*
* @author Mel Murphy (cumurphy@pdx.edu)
*
* Main function for the synthesizer program. This program runs on FreeRTOS
* with a 16 kB heap and a tick rate of 1000.
*
* @note
* The driver code and test application(s) for the pmodOLDrgb and pmodENC are
* based on code provided by Digilent, Inc.
******************************************************************************/

/**************************** Includes  *****************************************/

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
	// initialize hardware drivers
	portBASE_TYPE xStatus;	// return status for Xilinx driver functions
	xStatus = initialize_hardware();
	if (XST_SUCCESS != xStatus)
	{
		exit(1);
	}

	xil_printf("ECE 544 Final Project build test Program\n\r");
	xil_printf("By Mel Murphy, Jonathan Law, and David Hanson. Apr-2021\n\n\r");

	// Initialize semaphores
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
	initialize_system(&system_inst);	// initialize software modules
	menu_init(&menu_inst);				// initialize OLEDrgb display menu

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

// Calculate the constants used by the waveform generator
void calculate_constants(void* pvParameters)
{

	while(1)
	{
		if (xSemaphoreTake(calc_sem, minimum_wait))	// When signaled, compute constants used to define waveforms
		{
			compute_constants(0);	// compute constants for voices 0, 1, and 2
			compute_constants(1);
			compute_constants(2);
		}

		vTaskDelay(1);		// give up the rest of the time slice
	}

	vTaskDelete(NULL);
}

// Fill audio buffer with samples to send to I2S TX module
void calculate_samples(void* pvParameters)
{
	uint32_t samples[NUM_POLY];
	uint32_t mixedSample;

	while(1)
	{

		if (xSemaphoreTake(I2S_TX_low_sem, minimum_wait))
		{
			for (int index = 0; index < (BUFFER_SIZE >> 1); index++)		// calculate samples for the lower half of the buffer
			{
				samples[0] = pulseWave(0);	// generate a pulse wave for voices 0, 1, and 2
				samples[1] = pulseWave(1);
				samples[2] = pulseWave(2);
				mixedSample = samples[0] + samples[1] + samples[2];		// mix the samples for voices 0, 1, and 2
				bufferSample(mixedSample, index);						// send the samples to the sample buffer
			}
		}
		if (xSemaphoreTake(I2S_TX_high_sem, minimum_wait))
		{
			for (int index = (BUFFER_SIZE >> 1); index < BUFFER_SIZE; index++)	// calculate samples for the upper half of the buffer
			{
				samples[0] = pulseWave(0);	// generate a pulse wave for voices 0, 1, and 2
				samples[1] = pulseWave(1);
				samples[2] = pulseWave(2);
				mixedSample = samples[0] + samples[1] + samples[2];		// mix the samples for voices 0, 1, and 2
				bufferSample(mixedSample, index);						// send the samples to the sample buffer
			}
		}
	}

	vTaskDelete(NULL);
}

// Play notes, change steps, and change sequencer modes
void sequence_thread(void* pvParameters)
{
	mode_t mode;
	uint8_t btns;
	uint8_t encSw;

	while(1)
	{
		mode = getMode();	// get the current system mode
		if (xSemaphoreTake(timer_sem, minimum_wait))
		{
			if (mode == PLAY)
			{
				stop_note();	// send NOTE OFF for last sent MIDI note
				quiet_note(0);	// silence voice 0
				next_step();	// move to next sequencer step
				seq_timer();	// update sequence timer
				play_note();	// play current note by updating frequencies in signal generator
				xSemaphoreGive(calc_sem);	// send message to calculate constants task to compute constants for new frequency
				vTaskDelay(1);	// give up control to add space between MIDI messages so we don't overload the UART
				send_note();	// send MIDI ON note for current note
				updateLEDs();	// update LEDs to show new sequence step
			}
			else if (mode == RECORD)
			{
				updateLEDs();	// update LEDs for next blink
				toggleBlink();	// toggle LED on/off
			}
			else if (mode == BYPASS)
			{
				updateLEDs();	// update LEDs for next blink
				toggleBlink();	// toggle LED on/off
			}
			if (mode == MEMORY)
			{
				stop_note();	// send NOTE OFF for last sent MIDI note
				quiet_note(0);	// silence voice 0
				seq_timer();	// update sequence timer
				play_note();	// play current note by updating frequencies in signal generator
				xSemaphoreGive(calc_sem);	// send message to calculate constants task to compute constants for new frequency
				vTaskDelay(1);	// give up control to add space between MIDI messages so we don't overload the UART
				send_note();	// send MIDI ON note for current note
				updateLEDs();
			}
		}
		if (xSemaphoreTake(step_sem, minimum_wait))
		{
			btns = getButtons();	// get current button state
			if (mode == PLAY)
			{
				if (btns & BTNC)	// if center button pressed
				{
					setMode(RECORD);	// switch to RECORD mode
				}
			}
			else if (mode == RECORD)
			{
				if (btns & BTNC)	// if center button pressed
				{
					setMode(PLAY);	// switch to PLAY mode
				}
				else if (btns & BTNR)	// if right button pressed
				{
					stepSeqForward();	// move one sequence step to the right
				}
				else if (btns & BTNL)	// if left button pressed
				{
					stepSeqBackward();	// move one sequence step to the left
				}
			}
		}
		if (xSemaphoreTake(bypass_sem, minimum_wait))
		{
			encSw = (uint8_t)PMODENC544_getBtnSwReg() & ENC_SW_MASK;	// get rotary encoder switch state
			if (mode == PLAY || mode == RECORD || mode == MEMORY)
			{
				if (!encSw)	// if switch is in the up position (down position, but module is flipped when on the left side of the board
				{
					setMode(BYPASS);	// switch to BYPASS mode
				}
			}
			else if (mode == BYPASS)
			{
				if (encSw)	// if switch is in the down position (up position, but module is flipped)
				{
					setMode(RECORD);	// switch to RECORD mode
				}
			}
		}

		vTaskDelay(10);		// delay task--won't need to service sequencer for at least 10 ms
	}

	vTaskDelete(NULL);
}

// Navigate and update OLEDrgb display menu
static void menu_thread(void* pvParameters)
{
	while(1)
	{
		if (xSemaphoreTake(display_sem, minimum_wait))
		{
			menu_nav();	// run menu navigation function
		}

		vTaskDelay(1);	// give up the rest of time slice
	}

	vTaskDelete(NULL);
}

// Update MIDI states and signal generator frequencies dependent on those states
static void midi_thread(void* pvParameters)
{
	uint8_t mod;
	mode_t mode;
	uint32_t notes_hi;
	while(1)
	{
		mode = getMode();	// get currentsystem mode

		if (xSemaphoreTake(note_sem, minimum_wait))
		{
			if (mode == BYPASS || mode == MEMORY || mode == PLAY)
			{
				setNotes();	// get current state of MIDI notes from MIDI processor note data register
				getFreqs();	// update signal generator voices based on those notes
				xSemaphoreGive(calc_sem);	// send semaphore to tell thread to calculate new waveform constants
			}
			if (mode == RECORD)
			{
				notes_hi = MIDI_processor_getNotesHigh();
				if (((notes_hi & NOTE_0_MASK) >> 24) > getNote(7))
				{
					setNotePitch((notes_hi & NOTE_0_MASK) >> 24);
					stepSeqForward();
				}
				setNotes();	// get current state of MIDI notes from MIDI RX note data register
				getFreqs();	// update signal generator voices based on those notes
				xSemaphoreGive(calc_sem);// send semaphore to tell thread to calculate new waveform constants
			}
		}
		if (xSemaphoreTake(mod_sem, minimum_wait))
		{
			mod = MIDI_processor_getModulation();	// get modulation value from MIDI processor
			setModulationRX(mod);	// set modulation value in MIDI RX module
			setMod(0, mod);		// update modulation value of voices 0, 1, and 2
			setMod(1, mod);
			setMod(2, mod);
			xSemaphoreGive(calc_sem);
		}

		vTaskDelay(1);	// give up the rest of time slice
	}

	vTaskDelete(NULL);
}

