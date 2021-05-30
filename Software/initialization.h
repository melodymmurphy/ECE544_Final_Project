#ifndef _INITIALIZTION_H
#define _INITIALIZTION_H

/******************************* Includes **********************************/
#include "main.h"
#include "interrupts.h"

/****************************** Definitions ********************************/

#define TIMER_INIT 		((AXI_CLOCK_FREQ_HZ * 60) / 120)		// initialize timer value to 120 BPM

/************************** Function Prototypes ****************************/

int	initialize_hardware(void);
void shutdown_system(void);
int AXI_Timer_initialize(void);

#endif
