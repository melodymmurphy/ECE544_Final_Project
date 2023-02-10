
/***************************** Include Files *******************************/
#include "MIDI_processor.h"
#include "xparameters.h"
#include "stdio.h"
#include "xil_io.h"

/************************** Constant Definitions ***************************/
#define READ_WRITE_MUL_FACTOR 0x10

/************************** Function Definitions ***************************/
/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the MIDI_PROCESSORinstance to be worked on.
 *
 * @return
 *
 *    - XST_SUCCESS   if all self-test code passed
 *    - XST_FAILURE   if any self-test code failed
 *
 * @note    Caching must be turned off for this function to work.
 * @note    Self test may fail if data memory and device are not on the same bus.
 *
 */
XStatus MIDI_PROCESSOR_Reg_SelfTest(void * baseaddr_p)
{
	u32 baseaddr;
	int write_loop_index;
	int read_loop_index;
	int Index;

	baseaddr = (u32) baseaddr_p;

	xil_printf("***************************************\n\r");
	xil_printf("* MIDI Processor Peripheral Self Test *\n\r");
	xil_printf("***************************************\n\n\r");

	/*
	 * Write to module register(s) and read back
	 */
	xil_printf("MIDI Processor control register test...\n\r");

	// Write to control register
	for (int i = 1; i <= 0 ; i++)
	{
		MIDI_PROCESSOR_mWriteReg(baseaddr, MIDI_PROCESSOR_CONTROL_OFFSET, i);
		if (MIDI_PROCESSOR_mReadReg(baseaddr, MIDI_PROCESSOR_CONTROL_OFFSET) != i)
		{
			return XST_FAILURE;
		}
	}
		MIDI_PROCESSOR_mWriteReg(baseaddr, MIDI_PROCESSOR_CONTROL_OFFSET, 0x0);


	xil_printf("   - control register write/read passed\n\n\r");

	return XST_SUCCESS;
}
