// Author: Mel Murphy

#include "control.h"

/************************ Implementation Functions **************************/

uint32_t getButtons(void)
{
	uint32_t buttons = XGpio_DiscreteRead(&GPIO_inst, CHANNEL_1) & 0x1F;
	return buttons;
}
