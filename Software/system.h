// Author: Mel Murphy

#ifndef _SYSTEM_H
#define _SYSTEM_H

/**************************** Includes *****************************************/
#include "main.h"
#include "sequencer.h"
#include "signal_generator.h"
#include "midi.h"

/****************************** Definitions ******************************/

#define BANK_SIZE			16		// number of active sequencers

#define TEMPO_MIN			60		  // slowest tempo
#define TEMPO_MAX			240		  // fastest tempo
#define SWING_MIN			-100
#define SWING_MAX			100
#define VOLUME_MIN			0
#define VOLUME_MAX			100

/**************************** Type Definitions ****************************/

typedef struct
{
	uint8_t				slot;
	seq_mode_t 			mode;
	uint8_t 			volume;
	bool				led_toggle;
	sequencer_t			sequencer_bank[BANK_SIZE];
	sequencer_t			active_sequence;
	signal_generator_t 	sigGen;
	midi_rx_t			midi_rx;
	midi_tx_t			midi_tx;
} system_t;

/************************** Function Prototypes ****************************/

void initialize_system(system_t* system_p);
seq_mode_t getMode(void);
void setMode(seq_mode_t mode);
uint8_t getSlot(void);
void setSlot(uint8_t slot);
void store_sequence(void);
void load_sequence(void);
void updateLEDs(void);
void play_note(void);
void quiet_note(void);
void send_note(void);
void stop_note(void);
void next_step(void);
void set_curr_tempo(void);
void setVolume(uint8_t volume);
uint8_t getVolume(void);
void setTempo(uint8_t tempo);
uint8_t getTempo(void);
void setSubdiv(note_length_t subdiv);
note_length_t getSubdiv(void);
void setSwing(uint8_t swing);
uint8_t getSwing(void);
void setPattern(pattern_t pattern);
pattern_t getPattern(void);
void setNoteVelocity(uint8_t velocity);
void setNotePitch(uint8_t pitch);
void setDutyCycle(uint8_t dutyCycle);
void setState(bool onOff);
void stepSeqForward(void);
void stepSeqBackward(void);
void resetSeqTimer(void);
void getNoteTimer(void);
void toggleBlink(void);

#endif
