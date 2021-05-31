// Author: Mel Murphy

#include "initialization.h"

static XUartNs550Format midi_format;

/****************************************************************************
* initialize the system
*
* This function is executed once at start-up and after resets.  It initializes
* the peripherals
*****************************************************************************/

int	initialize_hardware(void)
{

	uint32_t status;				// status from Xilinx Lib calls

	// initialize the Nexys4 driver and (some of)the devices
	status = (uint32_t) NX4IO_initialize(NX4IO_BASEADDR);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// Write to SSD
	NX4IO_SSEG_setSSEG_DATA(SSEGHI, 0x0058E30E);
	NX4IO_SSEG_setSSEG_DATA(SSEGLO, 0x00144116);

	// enable the PWM outputs
	NX4IO_RGBLED_setChnlEn(RGB1, true, true, true);
	NX4IO_RGBLED_setChnlEn(RGB2, true, true, true);

	// initialize OLEDrgb display
	OLEDrgb_begin(&pmodOLEDrgb_inst, RGBDSPLY_GPIO_BASEADDR, RGBDSPLY_SPI_BASEADDR);

	// initialize the GPIO instances
	status = (uint32_t) XGpio_Initialize(&GPIO_inst, GPIO_0_DEVICE_ID);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	// GPIO0 channel 1 is a 21-bit input port.
	// GPIO0 channel 2 is a 16-bit output port
	XGpio_SetDataDirection(&GPIO_inst, GPIO_0_INPUT_0_CHANNEL, GPIO_IN_MASK);
	XGpio_SetDataDirection(&GPIO_inst, GPIO_0_OUTPUT_0_CHANNEL, GPIO_OUT_MASK);

	// initialize the PMOD rotary encoder driver and the device
	status = (uint32_t) PMODENC544_initialize(PMODENC_BASEADDR);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// Initialize the UART
	XUartNs550_Initialize(&UART_inst, UART_MIDI_TX_DEVICE_ID);

	// set up data format struct to pass to function
	midi_format.BaudRate = MIDI_BAUD_RATE;
	midi_format.DataBits = XUN_FORMAT_8_BITS;
	midi_format.Parity = XUN_FORMAT_NO_PARITY;
	midi_format.StopBits = XUN_FORMAT_1_STOP_BIT;

	// set data format of UART
	XUartNs550_SetDataFormat(&UART_inst, &midi_format);

	// initialize the AXI timer
	status = (uint32_t) AXI_Timer_initialize();
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// initialize the MIDI input processor
	status = (uint32_t) MIDI_processor_initialize(MIDI_RECEIVER_BASEADDR);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// Initialize I2S transmitter module;
	status = (uint32_t) I2S2_initialize(XPAR_I2S2_0_S00_AXI_BASEADDR);
	if (status != XST_SUCCESS)
	{
			return XST_FAILURE;
	}

	status = (uint32_t) initialize_interrupts();
	if (status != XST_SUCCESS)
	{
			return XST_FAILURE;
	}

	return XST_SUCCESS;

}


// Initialize AXI timer
int AXI_Timer_initialize(void){

	uint32_t status;				// status from Xilinx Lib calls
	uint32_t ctlsts;		// control/status register or mask

	status = XTmrCtr_Initialize(&AXITimerInst, AXI_TIMER_DEVICE_ID);
			if (status != XST_SUCCESS) {
				return XST_FAILURE;
			}
		status = XTmrCtr_SelfTest(&AXITimerInst, TIMER_1);
			if (status != XST_SUCCESS) {
				return XST_FAILURE;
			}

		ctlsts = XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_LOAD_MASK | XTC_CSR_DOWN_COUNT_MASK ;
		XTmrCtr_SetControlStatusReg(AXI_TIMER_BASEADDR, TIMER_1 ,ctlsts);

		//Set the value that is loaded into the timer counter and cause it to be loaded into the timer counter
		XTmrCtr_SetLoadReg(AXI_TIMER_BASEADDR, TIMER_1 , TIMER_INIT);
		XTmrCtr_LoadTimerCounterReg(AXI_TIMER_BASEADDR, TIMER_1);
		ctlsts = XTmrCtr_GetControlStatusReg(AXI_TIMER_BASEADDR, TIMER_1);
		ctlsts &= (~XTC_CSR_LOAD_MASK);
		XTmrCtr_SetControlStatusReg(AXI_TIMER_BASEADDR, TIMER_1, ctlsts);

		ctlsts = XTmrCtr_GetControlStatusReg(AXI_TIMER_BASEADDR, TIMER_1);
		ctlsts |= XTC_CSR_ENABLE_TMR_MASK;
		XTmrCtr_SetControlStatusReg(AXI_TIMER_BASEADDR, TIMER_1, ctlsts);

		XTmrCtr_Enable(AXI_TIMER_BASEADDR, TIMER_1);

	return XST_SUCCESS;

}



void shutdown_system(void)
{
	// clear the displays and power down the pmodOLEDrbg
	NX410_SSEG_setAllDigits(SSEGHI, CC_BLANK, CC_B, CC_LCY, CC_E, DP_NONE);
	NX410_SSEG_setAllDigits(SSEGLO, CC_B, CC_LCY, CC_E, CC_BLANK, DP_NONE);
	OLEDrgb_Clear(&pmodOLEDrgb_inst);
	OLEDrgb_end(&pmodOLEDrgb_inst);

	return;
}

