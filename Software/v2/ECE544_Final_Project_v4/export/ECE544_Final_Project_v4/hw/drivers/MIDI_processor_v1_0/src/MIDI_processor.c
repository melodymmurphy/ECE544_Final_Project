

/***************************** Include Files *******************************/
#include "MIDI_processor.h"

/***************************** Global variables ****************************/
static uint32_t baseAddress = 0L;
static bool isInitialized = false;

/************************** Function Definitions ***************************/
/**
 * Initializes the MIDI_processor peripheral and runs the self-test
 *
 * @param   baseaddr_p  base address of the MIDI_processor peripheral
 *
 * @return  returns XST_SUCCESS if the MIDI_processor is intialized, false otherwise
 *
 */
XStatus MIDI_processor_initialize(uint32_t baseaddr_p)
{
    XStatus sts;

    if (baseaddr_p == NULL) {
        isInitialized = false;
        return XST_FAILURE;
    }

    if (isInitialized) {
        return XST_SUCCESS;
    }
    else {
        baseAddress = baseaddr_p;
        sts = MIDI_PROCESSOR_Reg_SelfTest(baseAddress);
        if (sts != XST_SUCCESS)
            return XST_FAILURE;
        MIDI_processor_clearDisableInterrupts();
        isInitialized = true;
    }
    return XST_SUCCESS;
}

uint32_t MIDI_processor_getBaseAddress(void)
{
	return baseAddress;
}

uint32_t MIDI_processor_getNotesLow(void)
{
	return MIDI_PROCESSOR_mReadReg(baseAddress, MIDI_PROCESSOR_NOTE_DATA_LOW);
}

uint32_t MIDI_processor_getNotesHigh(void)
{
	return MIDI_PROCESSOR_mReadReg(baseAddress, MIDI_PROCESSOR_NOTE_DATA_HIGH);
}

uint8_t MIDI_processor_getModulation(void)
{
	return MIDI_PROCESSOR_mReadReg(baseAddress, MIDI_PROCESSOR_MODULATION_OFFSET);
}

void MIDI_processor_setControl(uint8_t data)
{
	uint32_t padding = 0;

	MIDI_PROCESSOR_mWriteReg(baseAddress, MIDI_PROCESSOR_CONTROL_OFFSET, (padding | data));

	return;
}

uint8_t MIDI_processor_readControl(void)
{
	return MIDI_PROCESSOR_mReadReg(baseAddress, MIDI_PROCESSOR_CONTROL_OFFSET);
}

uint8_t MIDI_processor_readInterrupts(void)
{
	return MIDI_PROCESSOR_mReadReg(baseAddress, MIDI_PROCESSOR_INTERRUPT_OFFSET);
}

void MIDI_processor_clearDisableInterrupts(void)
{
	MIDI_PROCESSOR_mWriteReg(baseAddress, MIDI_PROCESSOR_CONTROL_OFFSET, 0x2);

	return;
}

void MIDI_processor_enableInterrupts(void)
{
	MIDI_PROCESSOR_mWriteReg(baseAddress, MIDI_PROCESSOR_CONTROL_OFFSET, 0x1);

	return;
}
