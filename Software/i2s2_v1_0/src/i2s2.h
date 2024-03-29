
#ifndef I2S2_H
#define I2S2_H


/****************** Include Files ********************/
#include <stdbool.h>
#include "xil_types.h"
#include "xil_io.h"
#include "xstatus.h"

#define I2S2_TX_SEND_SAMPLE_REG 0
#define I2S2_TX_UNUSED0 4
#define I2S2_TX_UNUSED1 8
#define I2S2_TX_UNUSED2 12


/**************************** Type Definitions *****************************/
/**
 *
 * Write a value to a I2S2 register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the I2S2device.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void I2S2_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 *
 */
#define I2S2_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 *
 * Read a value from a I2S2 register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the I2S2 device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	u32 I2S2_mReadReg(u32 BaseAddress, unsigned RegOffset)
 *
 */
#define I2S2_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

/************************** Function Prototypes ****************************/
/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the I2S2 instance to be worked on.
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
XStatus I2S2_Reg_SelfTest(uint32_t baseaddr_p);
XStatus I2S2_initialize(uint32_t baseaddr_p);
void I2S2_Send_Sample(uint32_t);

#endif // I2S2_H
