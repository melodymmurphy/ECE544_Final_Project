// Author: Mel Murphy

#include "interrupts.h"

uint32_t initialize_interrupts(void)
{
	uint32_t status;		// function return status

	xil_printf("\n\n\n=== Initializing Interrupts ===\r\n\n\n");

	// connect MIDI input processor interrupt to interrupt controller
	status = xPortInstallInterruptHandler(MIDI_INTERRUPT_ID, (XInterruptHandler)MIDI_Handler, NULL);
	if (status != pdPASS) {
		return XST_FAILURE;
	}

	MIDI_processor_enableInterrupts();	// enable MIDI input interrupts

	// connect GPIO interrupt to interrupt controller
	status = xPortInstallInterruptHandler(GPIO_0_INTERRUPT_ID, (XInterruptHandler)GPIO_Handler, NULL);
	if (status != pdPASS) {
		return XST_FAILURE;
	}

	XGpio_InterruptEnable( &GPIO_inst, 0x1FFFF );
	XGpio_InterruptGlobalEnable( &GPIO_inst );

	// connect I2S transmit interrupt to interrupt controller
	status = xPortInstallInterruptHandler(I2S_TX_INTERRUPT_ID, (XInterruptHandler)I2S_TX_Handler, NULL);
	if (status != pdPASS) {
		return XST_FAILURE;
	}

	// connect PMOD rotary encoder interrupt to interrupt controller
	status = xPortInstallInterruptHandler(PMODENC_INTERRUPT_ID, (XInterruptHandler)ENC_Handler, NULL);
	if (status != pdPASS) {
		return XST_FAILURE;
	}

	// connect AXI timer interrupt to interrupt controller
	status = xPortInstallInterruptHandler(TIMER_INTERRUPT_ID, (XInterruptHandler)Timer_Handler, (void*)(&AXITimerInst));
	if (status != pdPASS) {
		return XST_FAILURE;
	}

	// enable AXI timer interrupts
	XTmrCtr_EnableIntr(XPAR_AXI_TIMER_1_BASEADDR, TIMER_1);

	// enable interrupts
	vPortEnableInterrupt(MIDI_INTERRUPT_ID);
	vPortEnableInterrupt(GPIO_0_INTERRUPT_ID);
	vPortEnableInterrupt(I2S_TX_INTERRUPT_ID);
	vPortEnableInterrupt(PMODENC_INTERRUPT_ID);
	vPortEnableInterrupt(TIMER_INTERRUPT_ID);

	microblaze_enable_interrupts();

	return XST_SUCCESS;
}

/**************************** Interrupt Handlers ******************************/

void Timer_Handler(void)
{
	uint32_t ctlsts;

	XTmrCtr_DisableIntr(AXITimerInst.BaseAddress, TIMER_1);
	ctlsts = XTmrCtr_GetControlStatusReg(AXITimerInst.BaseAddress, TIMER_1);

	xSemaphoreGiveFromISR(step_sem, NULL);

	XTmrCtr_WriteReg(AXITimerInst.BaseAddress, TIMER_1, XTC_TCSR_OFFSET, ctlsts | XTC_CSR_INT_OCCURED_MASK);	// Acknowledge interrupt
	XTmrCtr_EnableIntr(AXITimerInst.BaseAddress, TIMER_1);
}

void MIDI_Handler(void)
{
	// driver test
	uint32_t reg = MIDI_processor_readInterrupts();
	MIDI_processor_clearDisableInterrupts();
	bool noteOn = MIDI_processor_getNoteOn();
	uint32_t notereg = MIDI_processor_getNote();
	uint32_t modreg = MIDI_processor_getModulation();

	if (reg & MIDI_INT_MASK)	// if active interrupt
	{
		if (reg & NOTE_ON_MASK)	// note on
		{
		}
		else if (reg & NOTE_OFF_MASK)
		{
		}
		else if (reg & MODULATION_MASK)
		{
		}
	}

	MIDI_processor_enableInterrupts();	// reenable interrupts
}

void GPIO_Handler(void)
{
	xSemaphoreGiveFromISR(display_sem, NULL);
	XGpio_InterruptClear( &GPIO_inst, GPIO_IN_MASK );	// clear GPIO interrupts
}

void I2S_TX_Handler(void)
{
	int index = incrementSample();
	sendSample();

	if (index == 0)
	{
		xSemaphoreGiveFromISR(I2S_TX_high_sem, NULL);
	}
	if (index == (BUFFER_SIZE >> 1))
	{
		xSemaphoreGiveFromISR(I2S_TX_low_sem, NULL);
	}


	return;
}

void ENC_Handler(void)
{
	xSemaphoreGiveFromISR(display_sem, NULL);
}
