// Author: Mel Murphy

#include "control.h"

/************************ Implementation Functions **************************/

// get current state of pushbuttons
uint8_t getButtons(void)
{
	uint32_t buttons = (uint8_t)(XGpio_DiscreteRead(&GPIO_inst, CHANNEL_1) & GPIO_BUTTONS_MASK);
	return buttons;
}

// get current state of switches
uint16_t getSwitches(void)
{
	uint32_t switches = (uint16_t)((XGpio_DiscreteRead(&GPIO_inst, CHANNEL_1) & GPIO_SWITCHES_MASK) >> 5);
	return switches;
}
