
#ifndef SIG_GEN_H
#define SIG_GEN_H


/****************** Include Files ********************/
#include <stdbool.h>
#include "xil_io.h"
#include "xil_types.h"
#include "xstatus.h"

#define SIG_GEN_S00_AXI_SLV_REG0_OFFSET 0
#define SIG_GEN_S00_AXI_SLV_REG1_OFFSET 4
#define SIG_GEN_S00_AXI_SLV_REG2_OFFSET 8
#define SIG_GEN_S00_AXI_SLV_REG3_OFFSET 12
#define SIG_GEN_S00_AXI_SLV_REG4_OFFSET 16
#define SIG_GEN_S00_AXI_SLV_REG5_OFFSET 20
#define SIG_GEN_S00_AXI_SLV_REG6_OFFSET 24
#define SIG_GEN_S00_AXI_SLV_REG7_OFFSET 28

/*
.frequency  (slv_reg0),
.amplitude  (slv_reg1),
.mask       (slv_reg2),
.extra      (slv_reg3),
.sample     (sample)
*/


#define SIG_GEN_FREQUENCY 	SIG_GEN_S00_AXI_SLV_REG0_OFFSET
#define SIG_GEN_AMPLITUDE 	SIG_GEN_S00_AXI_SLV_REG1_OFFSET
#define SIG_GEN_MASK 		SIG_GEN_S00_AXI_SLV_REG2_OFFSET
#define SIG_GEN_EXTRA 		SIG_GEN_S00_AXI_SLV_REG3_OFFSET
//#define SIG_GEN_S00_AXI_SLV_REG4_OFFSET 16
//#define SIG_GEN_S00_AXI_SLV_REG5_OFFSET 20
//#define SIG_GEN_S00_AXI_SLV_REG6_OFFSET 24
#define SIG_GEN_SAMPLE 		SIG_GEN_S00_AXI_SLV_REG7_OFFSET

#define SAMPLE_RATE 	48000
#define BIT_DEPTH		24
#define MAX_AMP 		((1 << BIT_DEPTH) - 1)
#define DC_MIN			0
#define DC_MAX			127
#define RC_MIN			0
#define RC_MAX			127
#define AMP_MIN			0
#define AMP_MAX			127
#define NOTE_A4			440		// frequency value of MIDI note A4
#define MAX_SAW  		3
#define MAX_PULSE		4
#define NUM_POLY		4
#define BUFFER_SIZE		8192

/**************************** Type Definitions *****************************/
typedef struct
{
	uint8_t velocity[NUM_POLY];
	float frequency[NUM_POLY];
	float cycle[NUM_POLY];
	float high_cycle[NUM_POLY];
	float low_cycle[NUM_POLY];
	float rampUp[NUM_POLY];
	float rampDown[NUM_POLY];
	uint32_t peak[NUM_POLY];
	volatile uint32_t c[NUM_POLY];			// sample counter
	uint16_t calcIndex;
	volatile uint16_t readIndex;
	uint32_t sampleBuffer[BUFFER_SIZE];
} signal_generator_t;
/**
 *
 * Write a value to a SIG_GEN register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the SIG_GENdevice.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void SIG_GEN_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 *
 */
#define SIG_GEN_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 *
 * Read a value from a SIG_GEN register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the SIG_GEN device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	u32 SIG_GEN_mReadReg(u32 BaseAddress, unsigned RegOffset)
 *
 */
#define SIG_GEN_mReadReg(BaseAddress, RegOffset) \
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
 * @param   baseaddr_p is the base address of the SIG_GEN instance to be worked on.
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
XStatus SIG_GEN_Reg_SelfTest(void * baseaddr_p);
XStatus SIG_GEN_initialize(uint32_t baseaddr_p);

void setFrequency(uint32_t f);
uint32_t getFreq(void);

void setAmplitude(uint32_t f);
uint32_t getAmplitude(void);

void setMask(uint32_t f);
uint32_t getMask(void);

void setExtra(uint32_t f);
uint32_t getExtra(void);

void setSample(uint32_t f);
uint32_t getSample(void);

uint32_t tri_ramp(uint32_t f, uint32_t amp, uint32_t i, uint32_t rise_cycle);

#endif // SIG_GEN_H
