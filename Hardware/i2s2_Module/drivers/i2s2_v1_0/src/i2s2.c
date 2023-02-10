

/***************************** Include Files *******************************/
#include "i2s2.h"

/***************************** Global variables ****************************/
static uint32_t baseAddress = 0L;
static bool isInitialized = false;

/************************** Function Definitions ***************************/


/**
 * Initializes the I2S2 peripheral and runs the self-test
 *
 * @param   baseaddr_p  base address of the I2S2 peripheral
 *
 * @return  returns XST_SUCCESS if the I2S2 module is intialized, false
 * 			otherwise
 *
 */
XStatus I2S2_initialize(uint32_t baseaddr_p)
{
	XStatus xsts;
    if (baseaddr_p == NULL) {
        isInitialized = false;
        return XST_FAILURE;
    }
    else if (I2S2_Reg_SelfTest(baseaddr_p) == XST_SUCCESS)
    {
    	isInitialized = true;
    	baseAddress = baseaddr_p;
    	return XST_SUCCESS;
    }
    else
    {
    	return XST_FAILURE;
    }

}

/**
 *
 * Send a sample to the I2S2 module. This is best achieved on an interrupt
 * basis--the synchronizer will send an edge-triggered interrupt when the
 * I2S clock is ready for a new sample. Samples for this module should be
 * designed for a 48 kHz frequency, 24-bit depth application.
 *
 * @param   BaseAddress is the base address of the I2S2 device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  This function returns no data.
 *

 */
void I2S2_Send_Sample(uint32_t sample)
{
	if (isInitialized)
	{
		I2S2_mWriteReg(baseAddress, I2S2_TX_SEND_SAMPLE_REG, sample);
	}
	return;
}
