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


/************************** MAIN PROGRAM ************************************/
int main(void)
{
	flags.sample = false;		// initialize system flags
	flags.step = false;
	flags.quit = false;
	flags.button = false;
	flags.encoder = false;

    init_platform();	// initialize platform

	uint32_t sts;	// return status for Xilinx driver functions
	sts = initialize_hardware();
	if (XST_SUCCESS != sts)
	{
		exit(1);
	}

	xil_printf("ECE 544 Final Project build test Program\n\r");
	xil_printf("By Mel Murphy. Apr-2021\n\n\r");

	sequencer_t sequencer_inst;
	initialize_sequencer(&sequencer_inst);

	signal_generator_t sigGen_inst;
	initializeSigGen(&sigGen_inst);

	menu_main menu_inst;
	menu_init(&menu_inst);

	I2S2_Send_Sample(0);	// Send one sample to I2S output to trigger the ready interrupts

	uint8_t numSignals = 2;
	uint32_t signals[NUM_POLY];

	while(!flags.quit)
	{
		if (flags.step == true)
		{
			next_step();
			flags.step= false;
		}
		updateLEDs(sequencer_inst);

		if (flags.sample == true)
		{
			sigGen_inst.nextSample = mixer(numSignals, signals);
			flags.sample= false;
		}

		if ((flags.button == true) || (flags.encoder = true))
		{
			menu_nav();
			flags.button = false;
			flags.encoder = false;
		}

//		signals[0] = sawTriRampWave(110, 64, 64,  &(sigGen_inst.c[0]), 0);
//		signals[1] = sawTriRampWave(220, 64, 192, &(sigGen_inst.c[1]), 1);
//		signals[2] = sawTriRampWave(440, 64, 192, &(sigGen_inst.c[2]), 2);

//		signals[0] = pulseWave(220,    127, 127, &(sigGen_inst.c[0]), 0);
//		signals[1] = pulseWave(440,    127, 127, &(sigGen_inst.c[1]), 1);
//		signals[2] = pulseWave(880,    127, 127, &(sigGen_inst.c[2]), 2);
//		signals[3] = pulseWave(1318.5, 127, 127, &(sigGen_inst.c[3]), 3);

//		signals[0] = sawtoothWave(220, 127, &(sigGen_inst.c[0]), 0);
//		signals[1] = sawtoothWave(440, 127, &(sigGen_inst.c[1]), 1);
//		signals[2] = sawtoothWave(880, 127, &(sigGen_inst.c[2]), 2);

	}

	shutdown_system();

	// cleanup and exit
    cleanup_platform();
    exit(0);
}









