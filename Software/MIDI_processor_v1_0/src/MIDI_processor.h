
#ifndef MIDI_PROCESSOR_H
#define MIDI_PROCESSOR_H


/****************** Include Files ********************/
#include <stdbool.h>
#include "xil_types.h"
#include "xil_io.h"
#include "xstatus.h"

#define MIDI_PROCESSOR_NOTE_ON_OFFSET 		0
#define MIDI_PROCESSOR_NOTE_OFFSET 			4
#define MIDI_PROCESSOR_MODULATION_OFFSET 	8
#define MIDI_PROCESSOR_CONTROL_OFFSET 		12
#define MIDI_PROCESSOR_INTERRUPT_OFFSET 	16


/**************************** Type Definitions *****************************/
/**
 *
 * Write a value to a MIDI_PROCESSOR register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the MIDI_PROCESSORdevice.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void MIDI_PROCESSOR_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 *
 */
#define MIDI_PROCESSOR_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 *
 * Read a value from a MIDI_PROCESSOR register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the MIDI_PROCESSOR device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	u32 MIDI_PROCESSOR_mReadReg(u32 BaseAddress, unsigned RegOffset)
 *
 */
#define MIDI_PROCESSOR_mReadReg(BaseAddress, RegOffset) \
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
 * @param   baseaddr_p is the base address of the MIDI_PROCESSOR instance to be worked on.
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
XStatus MIDI_PROCESSOR_Reg_SelfTest(void * baseaddr_p);

XStatus MIDI_processor_initialize(uint32_t baseaddr_p);
uint32_t MIDI_processor_getBaseAddress(void);
bool MIDI_processor_getNoteOn(void);
uint8_t MIDI_processor_getNote(void);
uint8_t MIDI_processor_getModulation(void);
void MIDI_processor_setControl(uint8_t data);
uint8_t MIDI_processor_readControl(void);
uint8_t MIDI_processor_readInterrupts(void);
void MIDI_processor_clearDisableInterrupts(void);
void MIDI_processor_enableInterrupts(void);


#endif // MIDI_PROCESSOR_H
