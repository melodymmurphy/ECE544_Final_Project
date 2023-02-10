/**
*
* @file initialization.h
*
* @author Mel Murphy (cumurphy@pdx.edu)
*
* Header file for hardware and software module initialization
*
******************************************************************************/

#ifndef _INITIALIZTION_H
#define _INITIALIZTION_H

/******************************* Includes **********************************/
#include "main.h"
#include "interrupts.h"

/****************************** Definitions ********************************/

#define TIMER_INIT 		((AXI_CLOCK_FREQ_HZ * 60) / 80)		// initialize timer value to 80 BPM

/************************** Function Prototypes ****************************/

int	initialize_hardware(void);
void shutdown_system(void);
int AXI_Timer_initialize(void);

#endif
