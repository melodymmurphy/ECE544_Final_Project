
/***************************** Include Files *******************************/
#include "i2s2.h"
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
 * @param   baseaddr_p is the base address of the I2S2instance to be worked on.
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
XStatus I2S2_Reg_SelfTest(uint32_t baseaddr_p)
{
	u32 baseaddr;

	baseaddr = baseaddr_p;

	xil_printf("******************************\n\r");
	xil_printf("* I2S2 Module Self Test\n\r");
	xil_printf("******************************\n\n\r");

	/*
	 * Write to user logic module register(s) and read back
	 */
	xil_printf("User logic module test...\n\r");

	  I2S2_mWriteReg (baseaddr, 0, 0xABCDEF);
	  if ( I2S2_mReadReg (baseaddr, 0) != 0xABCDEF){
	    xil_printf ("Error reading register value at address %x\n", (int)baseaddr + 0);
	    return XST_FAILURE;
	  }

	xil_printf("   - register write/read passed\n\n\r");

	return XST_SUCCESS;
}
