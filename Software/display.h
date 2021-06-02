#ifndef _DISPLAY_H
#define _DISPLAY_H

/**************************** Includes *****************************************/

#include "main.h"


/**************************** Function Prototypes ******************************/

void PMDIO_itoa(int32_t value, char *string, int32_t radix);
void PMDIO_puthex(PmodOLEDrgb* InstancePtr, uint32_t num);
void PMDIO_putnum(PmodOLEDrgb* InstancePtr, int32_t num, int32_t radix);
uint16_t ByteSwap(uint16_t val);

#endif
