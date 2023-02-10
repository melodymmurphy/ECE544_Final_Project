/**
*
* @file interrupts.h
*
* @author Mel Murphy (cumurphy@pdx.edu)
*
* Header file for interrupt handlers and initialization
*
******************************************************************************/

#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

/**************************** Includes ********************************/

#include "main.h"
#include "sequencer.h"
#include "signal_generator.h"
#include "control.h"

/*************************** Definitions ******************************/

#define NOTE_MASK		0x1
#define MODULATION_MASK	0x2

/************************ Function Prototypes **************************/

uint32_t initialize_interrupts(void);
void Timer_Handler(void);
void MIDI_Handler(void);
void GPIO_Handler(void);
void I2S_TX_Handler(void);
void ENC_Handler(void);

#endif
