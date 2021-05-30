#ifndef _MENU_H
#define _MENU_H

/**************************** Includes *****************************************/

#include "main.h"
#include "display.h"
#include "sequencer.h"
#include "control.h"
#include "luts.h"

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
	u32 volume;
	pattern_t pattern;
} menu_sequence;

typedef struct {
	u16 velocity;
	u32 frequency;
	u8 dutycycle;
	bool state;
} menu_note;

typedef struct {
	seq_mode_t mode;
//	menu_memory memory;
	u8 slot;
	bool load;
	bool store;
} menu_system;

typedef struct {
	menu_system system;
	menu_sequence sequence;
	menu_note note;
} menu_main;

typedef enum {
	psystem, psequence, pnote, pcount
} _mmain;
typedef enum {
	smode, mslot, mload, mstore, scount
} _msystem;
//typedef enum {mslot,mload,mstore} _mmemory;
typedef enum {
	qtempo, qsubdivision, qswing, qvolume, qpattern, qcount
} _msequence;
typedef enum {
	nvelocity, nfrequency, ndutycycle, nstate, ncount
} _mnote;

/**************************** Macros ******************************/
#define menu_num(row,val)\
	OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 8, row);\
	OLEDrgb_PutString(&pmodOLEDrgb_inst,"    ");\
	OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 8, row);\
	PMDIO_putnum(&pmodOLEDrgb_inst, val,10);
#define menu_line(msel,row,str,val,type)\
	OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 1, row);\
	OLEDrgb_PutString(&pmodOLEDrgb_inst,str);\
	menu_##type(row,val);\
	msel[row-1] = &val;\
	row++;
#define menu_note_to_str(row,val)\
	OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 8, row);\
	OLEDrgb_PutString(&pmodOLEDrgb_inst,"    ");\
	OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 8, row);\
	OLEDrgb_PutString(&pmodOLEDrgb_inst, note_to_str(val));

#define menu_item(page,row) ((page<<4) | row)

/**************************** Function Prototypes ******************************/

void menu_init(menu_main *menu_p);
void menu_nav(void);
void menu_update(void);

#endif
