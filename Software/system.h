// Author: Mel Murphy

#ifndef _SYSTEM_H
#define _SYSTEM_H

/**************************** Includes *****************************************/
#include "main.h"
#include "sequencer.h"
#include "signal_generator.h"
#include "midi.h"

/****************************** Definitions ******************************/

#define BANK_SIZE					16		// number of active sequencers

/**************************** Type Definitions ****************************/

typedef struct
{
	sequencer_t			sequencer_bank[BANK_SIZE];
	uint8_t 			active_sequence;
	seq_mode_t 			mode;
	signal_generator_t 	sigGen;
	midi_rx_t			midi_rx;
	midi_tx_t			midi_tx;
} system_t;

/************************** Function Prototypes ****************************/

void initialize_system(system_t* system_p);
seq_mode_t getMode(void);
void setMode(seq_mode_t mode);
seq_mode_t getSlot(void);
void setSlot(uint8_t activeSequence);
void updateLEDs();
void play_note();
void send_note();
void stop_note();
void next_step();

#endif
