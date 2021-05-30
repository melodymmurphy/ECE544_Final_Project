// Author: Mel Murphy

#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

/**************************** Includes ********************************/

#include "main.h"
#include "sequencer.h"
#include "signal_generator.h"

/*************************** Definitions ******************************/

#define MIDI_INT_MASK	0x1
#define NOTE_ON_MASK	0x2
#define NOTE_OFF_MASK	0x4
#define MODULATION_MASK	0x8

/************************ Function Prototypes **************************/

uint32_t initialize_interrupts(void);
void Timer_Handler(void);
void MIDI_Handler(void);
void GPIO_Handler(void);
void I2S_TX_Handler(void);
void ENC_Handler(void);

#endif
