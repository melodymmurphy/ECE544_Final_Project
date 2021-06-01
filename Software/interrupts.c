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

	XTmrCtr_DisableIntr(AXITimerInst.BaseAddress, TIMER_1);		// disable interrupts
	ctlsts = XTmrCtr_GetControlStatusReg(AXITimerInst.BaseAddress, TIMER_1);	// get CSR

	xSemaphoreGiveFromISR(step_sem, NULL);	// notify task that sequencer should be updated

	XTmrCtr_WriteReg(AXITimerInst.BaseAddress, TIMER_1, XTC_TCSR_OFFSET, ctlsts | XTC_CSR_INT_OCCURED_MASK);
	XTmrCtr_EnableIntr(AXITimerInst.BaseAddress, TIMER_1);	// acknowledge interrupt, re-enable interrupts, start timer
}

void MIDI_Handler(void)
{
	uint32_t reg = MIDI_processor_readInterrupts();
	MIDI_processor_clearDisableInterrupts();

	if (reg & MIDI_INT_MASK)			// if active interrupt
	{
		if (reg & NOTE_ON_MASK)			// note on
		{
			xSemaphoreGiveFromISR(note_on_sem, NULL);
		}
		else if (reg & NOTE_OFF_MASK)	// note off
		{
			xSemaphoreGiveFromISR(note_off_sem, NULL);
		}
		else if (reg & MODULATION_MASK)	// modukatino
		{
			xSemaphoreGiveFromISR(mod_sem, NULL);
		}
	}

	MIDI_processor_enableInterrupts();
}

void GPIO_Handler(void)
{
	xSemaphoreGiveFromISR(display_sem, NULL);			// notify display that GPIO state has changed
	XGpio_InterruptClear( &GPIO_inst, GPIO_IN_MASK );	// clear GPIO interrupts
}

void I2S_TX_Handler(void)
{
	int index = incrementSample();						// move to next sample to read
	sendSample();										// send sample to I2S module

	if (index == 0)										// if reading low buffer
	{
		xSemaphoreGiveFromISR(I2S_TX_high_sem, NULL);	// notify task to fill high buffer
	}
	if (index == (BUFFER_SIZE >> 1))					// if reading high buffer
	{
		xSemaphoreGiveFromISR(I2S_TX_low_sem, NULL);	// send semaphore to write low buffer
	}


	return;
}

void ENC_Handler(void)
{
	xSemaphoreGiveFromISR(display_sem, NULL);			// notify task encoder state has changed
}
