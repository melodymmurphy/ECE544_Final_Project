/**
* @file menu.c
*
* @author Jonathan law
*
* @note
* menu for navigating and changing the parameters using the
* buttons, encoder, and oled display
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

/**************************** Includes *****************************************/
#include "menu.h"

/**************************** Globals *****************************************/
static menu_main *menu;
PmodOLEDrgb	pmodOLEDrgb_inst;
static u8 row=1,last_row=-1,row_max=1;
static u8 page=-1;
static u32 *menu_sel[6];
static s32 enc=0,last_enc=0;

/**************************** Functions *****************************************/

void menu_init(menu_main *menu_p) {

	menu = menu_p;

	OLEDrgb_SetFontColor(&pmodOLEDrgb_inst,OLEDrgb_BuildRGB(200, 12, 44));

	(*menu).system.mode = PLAY;
	(*menu).system.slot = 1;
	(*menu).system.load = 0;
	(*menu).system.store = 0;

	(*menu).sequence.tempo = 60;
	(*menu).sequence.subdivision = WHOLE;
	(*menu).sequence.swing = 0;
	(*menu).sequence.volume = 10;
	(*menu).sequence.pattern = FORWARD;

	(*menu).note.velocity = 0;
	(*menu).note.frequency = 0;
	(*menu).note.dutycycle = 0;
	(*menu).note.state = 0;

	PMODENC544_clearRotaryCount();
}

void menu_nav(void) {
	u8 r = 1;

	uint32_t btns = (getButtons() & GPIO_BUTTONS_MASK);
	// menu page change
	if (PMODENC544_isBtnPressed() | (page>=pcount)) {
		page++;
		if (page>=pcount)
			page = 0;

		OLEDrgb_Clear(&pmodOLEDrgb_inst);
		switch (page){
			case psystem:
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 0);
				OLEDrgb_PutString(&pmodOLEDrgb_inst,"system");
				menu_line(menu_sel,r,"mode:",(*menu).system.mode);
				menu_line(menu_sel,r,"slot:",(*menu).system.slot);
				menu_line(menu_sel,r,"load:",(*menu).system.load);
				menu_line(menu_sel,r,"store:",(*menu).system.store);
				row_max = scount;
				break;
			case psequence:
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 0);
				OLEDrgb_PutString(&pmodOLEDrgb_inst,"sequence");
				menu_line(menu_sel,r,"tempo:",(*menu).sequence.tempo);
				menu_line(menu_sel,r,"subdiv:",(*menu).sequence.subdivision);
				menu_line(menu_sel,r,"swing:",(*menu).sequence.swing);
				menu_line(menu_sel,r,"volume:",(*menu).sequence.volume);
				menu_line(menu_sel,r,"patt:",(*menu).sequence.pattern);
				row_max = qcount;
				break;
			case pnote:
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 0);
				OLEDrgb_PutString(&pmodOLEDrgb_inst,"note");
				menu_line(menu_sel,r,"vel:",(*menu).note.velocity);
				menu_line(menu_sel,r,"freq:",(*menu).note.frequency);
				menu_line(menu_sel,r,"dutyc:",(*menu).note.dutycycle);
				menu_line(menu_sel,r,"state:",(*menu).note.state);
				row_max = ncount;
				break;
		}
		row = 1;
		last_row = -1;
	}

	// cursor move
	if (btns & BTNU) {	// BTN U
		if (row>1)
			row = row - 1;					// row up
	} else if (btns & BTND) {	// BTN D
		if (row<row_max)
			row = row + 1;	// row down
	}

	if (row!=last_row) {
		OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, last_row);
		OLEDrgb_PutString(&pmodOLEDrgb_inst, " ");
		OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, row);
		OLEDrgb_PutString(&pmodOLEDrgb_inst, ">");
		last_row = row;
	}

	menu_update();
}

void menu_update(void) {
	s32 *sel;
	s32 min,max;

	// encoder change
	enc = 0-PMODENC544_getRotaryCount();
	if (enc==0) return;
	PMODENC544_clearRotaryCount();

	// change selected parameter
	sel=menu_sel[row-1];
	*sel = *sel + (enc);

	// parameter value limits
	switch (menu_item(page,row-1)){
		case menu_item(psystem,smode):
			min = 0;
			max = 3;
			break;
		case menu_item(psystem,mslot):
			min = 1;
			max = 16;
			break;
		case menu_item(psystem,mload):
		case menu_item(psystem,mstore):
		case menu_item(pnote,nstate):
			min = 0;
			max = 1;
			break;
		case menu_item(psequence,qtempo):
			min = 60;
			max = 240;
			break;
		case menu_item(psequence,qsubdivision):
			min = 0;
			max = 7;
			break;
		case menu_item(psequence,qswing):
			min = -100;
			max = 100;
			break;
		case menu_item(psequence,qvolume):
		case menu_item(pnote,ndutycycle):
			min = 0;
			max = 100;
			break;
		case menu_item(psequence,qpattern):
			min = 0;
			max = 4;
			break;
		case menu_item(pnote,nvelocity):
			min = 0;
			max = 127;
			break;
		case menu_item(pnote,nfrequency):
			min = 0;
			max = 24000;
			break;
	}
	if (*sel<min) *sel=min;
	else if (*sel>max) *sel=max;

	// update oled value
	menu_val(row,*sel);
	last_enc = enc;
}
