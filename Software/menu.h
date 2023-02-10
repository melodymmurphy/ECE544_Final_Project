/**
* @file menu.c
*
* @author Jonathan law
*
* @note
* Header file for menu for navigating and changing the parameters using
* the buttons, encoder, and oled display
*
* example:
// in main.c
#include "menu.h"
// create menu instance
menu_main menu;
menu_init(&menu);
// poll for button and encoder
menu_nav();
usleep(100000);
*/

#ifndef _MENU_H
#define _MENU_H

/**************************** Includes *****************************************/

#include "main.h"
#include "display.h"
#include "sequencer.h"
#include "control.h"
#include "luts.h"
#include "system.h"

/**************************** typedefs *****************************************/

//typedef struct {
//	u8		slots;
//	bool	load;
//	bool	store;
//} menu_memory;
typedef struct {
	u16 tempo;
	note_length_t subdivision;
	s32 swing;
	pattern_t pattern;
} menu_sequence;

//typedef struct {
//	u16 velocity;
//	u32 frequency;
//	u8 dutycycle;
//	bool state;
//} menu_note;

typedef struct {
	u32 volume;
	seq_mode_t mode;
//	menu_memory memory;
	u8 slot;
	bool load;
	bool store;
} menu_system;

typedef struct {
	menu_system system;
	menu_sequence sequence;
//	menu_note note;
} menu_main;

typedef enum {
	psystem, psequence, pcount
} _mmain;
typedef enum {
	svolume, smode, mslot, mload, mstore, scount
} _msystem;
//typedef enum {mslot,mload,mstore} _mmemory;
typedef enum {
	qtempo, qsubdivision, qswing, qpattern, qcount
} _msequence;
//typedef enum {
//	nvelocity, nfrequency, ndutycycle, nstate, ncount
//} _mnote;

/**************************** Macros ******************************/
#define colv	8
#define menu_val_erase(row)\
	OLEDrgb_SetCursor(&pmodOLEDrgb_inst, colv, row);\
	OLEDrgb_PutString(&pmodOLEDrgb_inst,"    ");
#define menu_num(row,val)\
	menu_val_erase(row);\
	OLEDrgb_SetCursor(&pmodOLEDrgb_inst, colv, row);\
	PMDIO_putnum(&pmodOLEDrgb_inst, val,10);
#define menu_line(msel,row,str,val,type)\
	OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 1, row);\
	OLEDrgb_PutString(&pmodOLEDrgb_inst,str);\
	menu_##type(row,val);\
	msel[row-1] = &val;\
	row++;
#define menu_note_to_str(row,val)\
	menu_val_erase(row);\
	OLEDrgb_SetCursor(&pmodOLEDrgb_inst, colv, row);\
	OLEDrgb_PutString(&pmodOLEDrgb_inst, note_to_str(val));
#define menu_seq_op(row,val)\
	menu_val_erase(row);\
	OLEDrgb_SetCursor(&pmodOLEDrgb_inst, colv, row);\
	OLEDrgb_PutString(&pmodOLEDrgb_inst, seq_op[val]);
#define menu_mode_str(row,val)\
	menu_val_erase(row);\
	OLEDrgb_SetCursor(&pmodOLEDrgb_inst, colv, row);\
	OLEDrgb_PutString(&pmodOLEDrgb_inst, mode_str[val]);

#define menu_item(page,row) ((page<<4) | row)

/**************************** Function Prototypes ******************************/

void menu_init(menu_main *menu_p);
void menu_nav(void);
void menu_update(void);

#endif
