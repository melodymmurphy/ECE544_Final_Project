

/***************************** Include Files *******************************/
#include "sig_gen.h"

/***************************** Global variables ****************************/
static uint32_t baseAddress = 0L;
static bool isInitialized = false;

/************************** Function Definitions ***************************/

XStatus SIG_GEN_initialize(uint32_t baseaddr_p)
{
    XStatus sts;

    xil_printf("Initializing SIG_GEN - \n\r");
    if (baseaddr_p == NULL) {
        isInitialized = false;
        return XST_FAILURE;
    }

    if (isInitialized) {
        return XST_SUCCESS;
    }
    else {
        baseAddress = baseaddr_p;
        sts = SIG_GEN_Reg_SelfTest(baseAddress);
        if (sts != XST_SUCCESS)
            return XST_FAILURE;
        isInitialized = true;
    }
    xil_printf("DONE\n\r");
    return XST_SUCCESS;
}

void setFrequency(uint32_t f)
{
	u32 reg = SIG_GEN_FREQUENCY;
	SIG_GEN_mWriteReg(baseAddress,reg,f);
}
uint32_t getFreq(void)
{
	u32 reg = SIG_GEN_FREQUENCY;
	return SIG_GEN_mReadReg(baseAddress,reg);
}

void setAmplitude(uint32_t f)
{
	u32 reg = SIG_GEN_AMPLITUDE;
	SIG_GEN_mWriteReg(baseAddress,reg,f);
}
uint32_t getAmplitude(void)
{
	u32 reg = SIG_GEN_AMPLITUDE;
	return SIG_GEN_mReadReg(baseAddress,reg);
}

void setMask(uint32_t f)
{
	u32 reg = SIG_GEN_MASK;
	SIG_GEN_mWriteReg(baseAddress,reg,f);
}
uint32_t getMask(void)
{
	u32 reg = SIG_GEN_MASK;
	return SIG_GEN_mReadReg(baseAddress,reg);
}

void setExtra(uint32_t f)
{
	u32 reg = SIG_GEN_EXTRA;
	SIG_GEN_mWriteReg(baseAddress,reg,f);
}
uint32_t getExtra(void)
{
	u32 reg = SIG_GEN_EXTRA;
	return SIG_GEN_mReadReg(baseAddress,reg);
}

void setSample(uint32_t f)
{
	u32 reg = SIG_GEN_SAMPLE;
	SIG_GEN_mWriteReg(baseAddress,reg,f);
}
uint32_t getSample(void)
{
	u32 reg = SIG_GEN_SAMPLE;
	return SIG_GEN_mReadReg(baseAddress,reg);
}

uint32_t tri_ramp(uint32_t f, uint32_t amp, uint32_t i, uint32_t rise_cycle)
{
	setFrequency(f);
	setAmplitude(amp);
	setMask(1<<i);
	setExtra(rise_cycle);
	return getSample();
}
