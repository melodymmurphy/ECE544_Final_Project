#include "interrupts.h"

uint32_t initialize_interrupts(void)
{
	uint32_t status;		// function return status

	// initialize the interrupt controller
	status = XIntc_Initialize(&IntrptCtlrInst, INTC_DEVICE_ID);
	if (status != XST_SUCCESS)
	{
	   return XST_FAILURE;
	}

	// connect AXI timer interrupt to interrupt controller
	status = XIntc_Connect(&IntrptCtlrInst, AXI_TIMER_INTC_VEC_ID,
				(XInterruptHandler)Timer_Handler,
				(void*)(&AXITimerInst));
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// enable AXI timer interrupts
	XTmrCtr_EnableIntr(XPAR_AXI_TIMER_0_BASEADDR, TIMER_1);

	// connect MIDI input processor interrupt to interrupt controller
	status = XIntc_Connect(&IntrptCtlrInst, XPAR_INTC_0_MIDI_PROCESSOR_0_VEC_ID,
				(XInterruptHandler)MIDI_Handler,
				NULL);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	MIDI_processor_enableInterrupts();	// enable MIDI input interrupts

	// connect GPIO interrupt to interrupt controller
	status = XIntc_Connect(&IntrptCtlrInst, XPAR_INTC_0_GPIO_0_VEC_ID,
				(XInterruptHandler)GPIO_Handler,
				NULL);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XGpio_InterruptEnable( &GPIO_inst, 0x1FFFF );
	XGpio_InterruptGlobalEnable( &GPIO_inst );

	// connect I2S transmit interrupt to interrupt controller
	status = XIntc_Connect(&IntrptCtlrInst, XPAR_INTC_0_I2S2_0_VEC_ID,
				(XInterruptHandler)I2S_TX_Handler,
				NULL);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// connect PMOD rotary encoder interrupt to interrupt controller
	status = XIntc_Connect(&IntrptCtlrInst, XPAR_INTC_0_PMODENC544_0_VEC_ID,
				(XInterruptHandler)ENC_Handler,
				NULL);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// start the interrupt controller such that interrupts are enabled for
	// all devices that cause interrupts.
	status = XIntc_Start(&IntrptCtlrInst, XIN_REAL_MODE);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// enable interrupts
	XIntc_Enable(&IntrptCtlrInst, TIMER_INTERRUPT_ID);
	XIntc_Enable(&IntrptCtlrInst, MIDI_INTERRUPT_ID);
	XIntc_Enable(&IntrptCtlrInst, GPIO_0_INTERRUPT_ID);
	XIntc_Enable(&IntrptCtlrInst, I2S_TX_INTERRUPT_ID);
	XIntc_Enable(&IntrptCtlrInst, PMODENC_INTERRUPT_ID);

	microblaze_enable_interrupts();

	return XST_SUCCESS;
}

/**************************** Interrupt Handlers ******************************/

void Timer_Handler(void)
{
	uint32_t ctlsts;

	XTmrCtr_DisableIntr(AXITimerInst.BaseAddress, TIMER_1);
	ctlsts = XTmrCtr_GetControlStatusReg(AXITimerInst.BaseAddress, TIMER_1);

	flags.step = true;

	XTmrCtr_WriteReg(AXITimerInst.BaseAddress, TIMER_1, XTC_TCSR_OFFSET, ctlsts | XTC_CSR_INT_OCCURED_MASK);	// Acknowledge interrupt
	XTmrCtr_EnableIntr(AXITimerInst.BaseAddress, TIMER_1);
}

void MIDI_Handler(void)
{
	// driver test
	uint32_t reg = MIDI_processor_readInterrupts();
	MIDI_processor_clearDisableInterrupts();
	xil_printf("Interrupts: %x\r\n", reg);
	bool noteOn = MIDI_processor_getNoteOn();
	uint32_t notereg = MIDI_processor_getNote();
	uint32_t modreg = MIDI_processor_getModulation();

	if (reg & MIDI_INT_MASK)	// if active interrupt
	{
		if (reg & NOTE_ON_MASK)	// note on
		{
			xil_printf("Note on %d: %d\r\n", noteOn, notereg);
		}
		else if (reg & NOTE_OFF_MASK)
		{
			xil_printf("Note on %d: %d\r\n", noteOn, notereg);
		}
		else if (reg & MODULATION_MASK)
		{
			xil_printf("Modulation: %d\r\n", modreg);
		}
	}

	MIDI_processor_enableInterrupts();	// reenable interrupts
}

void GPIO_Handler(void)
{
	flags.button = true;
	XGpio_InterruptClear( &GPIO_inst, GPIO_IN_MASK );	// clear GPIO interrupts
}

void I2S_TX_Handler(void)
{

	flags.sample = true;

	incrementSamples();
	sendSample();
}

void ENC_Handler(void)
{
	flags.encoder = true;
}
