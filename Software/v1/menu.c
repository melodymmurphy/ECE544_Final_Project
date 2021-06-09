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
//PmodOLEDrgb	pmodOLEDrgb_inst;
static u8 row=1,last_row=-1,row_max=1;
static u8 page=-1;
static u32 *menu_sel[6];
static s32 enc=0,last_enc=0;

/**************************** string LUT ******************************/
char* seq_op[2] = {"?","done"};
char* mode_str[4] = {"PLAY", "REC", "BYP", "MEM"};

/**************************** Functions *****************************************/

void menu_init(menu_main *menu_p) {

	menu = menu_p;

	OLEDrgb_SetFontColor(&pmodOLEDrgb_inst,OLEDrgb_BuildRGB(200, 12, 44));

	(*menu).system.volume = getVolume();
	(*menu).system.mode = getMode();
	(*menu).system.slot = getSlot();
	(*menu).system.load = 0;
	(*menu).system.store = 0;

	(*menu).sequence.tempo = getTempo();
	(*menu).sequence.subdivision = getSubdiv();
	(*menu).sequence.swing = getSwing();
	(*menu).sequence.pattern = getPattern();

//	(*menu).note.velocity = 0;
//	(*menu).note.frequency = 0;
//	(*menu).note.dutycycle = 0;
//	(*menu).note.state = 0;

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
				OLEDrgb_PutString(&pmodOLEDrgb_inst,"system      ");
				menu_line(menu_sel,r,"volume:",(*menu).system.volume,num);
				menu_line(menu_sel,r,"mode:  ",(*menu).system.mode,mode_str);
				menu_line(menu_sel,r,"slot:  ",(*menu).system.slot,num);
				menu_line(menu_sel,r,"load:  ",(*menu).system.load,seq_op);
				menu_line(menu_sel,r,"store: ",(*menu).system.store,seq_op);
				row_max = r-1;
				break;
			case psequence:
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 0);
				OLEDrgb_PutString(&pmodOLEDrgb_inst,"sequence    ");
				menu_line(menu_sel,r,"tempo: ",(*menu).sequence.tempo,num);
				menu_line(menu_sel,r,"subdiv:",(*menu).sequence.subdivision,num);
				menu_line(menu_sel,r,"swing: ",(*menu).sequence.swing,num);
				menu_line(menu_sel,r,"patt:  ",(*menu).sequence.pattern,num);
				row_max = r-1;
				break;
//			case pnote:
//				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 0);
//				OLEDrgb_PutString(&pmodOLEDrgb_inst,"note        ");
//				menu_line(menu_sel,r,"vel:   ",(*menu).note.velocity,num);
//				menu_line(menu_sel,r,"freq:  ",(*menu).note.frequency,note_to_str);
////				menu_line(menu_sel,r,"freq:",(*menu).note.frequency,num);
////					menu_str(r-1,note_to_str((*menu).note.frequency));
//				menu_line(menu_sel,r,"dutyc: ",(*menu).note.dutycycle,num);
//				menu_line(menu_sel,r,"state: ",(*menu).note.state,num);
//				row_max = r-1;
//				break;
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
//		case menu_item(pnote,nstate):
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
		case menu_item(psystem,svolume):
//		case menu_item(pnote,ndutycycle):
			min = 0;
			max = 100;
			break;
		case menu_item(psequence,qpattern):
			min = 0;
			max = 4;
			break;
//		case menu_item(pnote,nvelocity):
//		case menu_item(pnote,nfrequency):
//			min = 0;
//			max = 127;
//			break;
//			min = 0;
//			max = 24000;
//			break;
	}
	if (*sel<min) *sel=min;
	else if (*sel>max) *sel=max;

	// update oled and system API value
	switch (menu_item(page,row-1)) {
		case menu_item(psystem,mload):
			if (!*sel) break;		// 0 skip
			load_sequence();
			menu_seq_op(row,*sel);		// display op done
			*sel=min;					// reset val
			break;
		case menu_item(psystem,mstore):
			if (!*sel) break;		// 0 skip
			store_sequence();
			menu_seq_op(row,*sel);		// display op done
			*sel=min;					// reset val
			break;
		case menu_item(psystem,smode):
			setMode(*sel);
			menu_mode_str(row,*sel);		// display op done
			break;
//		case menu_item(pnote,nfrequency):
//			menu_note_to_str(row,*sel);
//			break;
		default:
			// oled number
			menu_num(row,*sel);
			// set API for number value
			switch (menu_item(page,row-1)) {
				case menu_item(psystem,mslot):
					setSlot(*sel);
					break;
				case menu_item(psequence,qtempo):
					setTempo(*sel);
					break;
				case menu_item(psequence,qsubdivision):
					setSubdiv(*sel);
					break;
				case menu_item(psequence,qswing):
					setSwing(*sel);
					break;
				case menu_item(psystem,svolume):
					setVolume(*sel);
					break;
				case menu_item(psequence,qpattern):
					setPattern(*sel);
					break;
				default:
					break;
			}
			break;	// outer switch default
	}

	last_enc = enc;
}
