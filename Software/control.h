/**
*
* @file control.h
*
* @author Mel Murphy (cumurphy@pdx.edu)
*
* Header file for GPIO input control functions
*
******************************************************************************/

#ifndef _CONTROL_H
#define _CONTROL_H

/**************************** Includes *****************************************/

#include "main.h"

/**************************** Function Prototypes ******************************/

uint8_t getButtons(void);
uint16_t getSwitches(void);

#endif
