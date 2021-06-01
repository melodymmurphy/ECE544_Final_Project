// Author: Mel Murphy

#ifndef _MAIN_H
#define _MAIN_H

/**************************** Includes  *****************************************/

// Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

// Kernel includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

// BSP Includes
#include "sleep.h"
#include "xparameters.h"
#include "xstatus.h"
#include "xil_types.h"
#include "xil_io.h"
#include "microblaze_sleep.h"

// Drivers
#include "nexys4IO.h"
#include "PmodOLEDrgb.h"
#include "PmodENC544.h"
#include "xgpio.h"
#include "xintc.h"
#include "xtmrctr.h"
#include "MIDI_processor.h"
#include "xuartns550.h"
#include "i2s2.h"


/************************** Defines ****************************/

// Peripheral Parameters

// Clock frequencies
#define CPU_CLOCK_FREQ_HZ			XPAR_CPU_CORE_CLOCK_FREQ_HZ
#define AXI_CLOCK_FREQ_HZ			XPAR_CPU_M_AXI_DP_FREQ_HZ

// AXI timer parameters
#define AXI_TIMER_DEVICE_ID			XPAR_AXI_TIMER_1_DEVICE_ID
#define AXI_TIMER_BASEADDR			XPAR_AXI_TIMER_1_BASEADDR
#define AXI_TIMER_HIGHADDR			XPAR_AXI_TIMER_1_HIGHADDR
#define AXI_TIMER_INTC_VEC_ID 		XPAR_INTC_0_TMRCTR_1_VEC_ID
#define TIMER_1						0
#define TIMER_2						1

// Definitions for peripheral NEXYS4IO
#define NX4IO_DEVICE_ID				XPAR_NEXYS4IO_0_DEVICE_ID
#define NX4IO_BASEADDR				XPAR_NEXYS4IO_0_S00_AXI_BASEADDR
#define NX4IO_HIGHADDR				XPAR_NEXYS4IO_0_S00_AXI_HIGHADDR

// Definitions for peripheral PMODOLEDRGB
#define RGBDSPLY_DEVICE_ID			XPAR_PMODOLEDRGB_0_DEVICE_ID
#define RGBDSPLY_GPIO_BASEADDR		XPAR_PMODOLEDRGB_0_AXI_LITE_GPIO_BASEADDR
#define RGBDSPLY_GPIO_HIGHADDR		XPAR_PMODOLEDRGB_0_AXI_LITE_GPIO_HIGHADD
#define RGBDSPLY_SPI_BASEADDR		XPAR_PMODOLEDRGB_0_AXI_LITE_SPI_BASEADDR
#define RGBDSPLY_SPI_HIGHADDR		XPAR_PMODOLEDRGB_0_AXI_LITE_SPI_HIGHADDR

// Definitions for peripheral PMODENC
#define PMODENC_DEVICE_ID			XPAR_PMODENC544_0_DEVICE_ID
#define PMODENC_BASEADDR			XPAR_PMODENC544_0_S00_AXI_BASEADDR
#define PMODENC_HIGHADDR			XPAR_PMODENC544_0_S00_AXI_HIGHADDR
#define PMODENC_INTERRUPT_ID		XPAR_MICROBLAZE_0_AXI_INTC_PMODENC544_0_INT_SIG_INTR

// GPIO parameters
#define GPIO_0_DEVICE_ID			XPAR_AXI_GPIO_0_DEVICE_ID
#define GPIO_0_INPUT_0_CHANNEL		1
#define GPIO_0_OUTPUT_0_CHANNEL		2
#define GPIO_0_INTERRUPT_ID			XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_0_IP2INTC_IRPT_INTR
#define GPIO_IN_MASK				0x1FFFF
#define GPIO_OUT_MASK				0x0000
#define GPIO_BUTTONS_MASK			0x00001F
#define GPIO_SWITCHES_MASK			0x1FFFE0
#define GPIO_LEDS_MASK				0xFFFF
#define CHANNEL_1					1
#define CHANNEL_2					2
#define BTNU						0x01
#define BTNL						0x02
#define BTNR						0x04
#define BTND						0x08
#define BTNC						0x10

// MIDI Receiver
#define MIDI_RECEIVER_BASEADDR 		XPAR_MIDI_PROCESSOR_0_S00_AXI_BASEADDR
#define MIDI_INTERRUPT_ID	  		XPAR_MICROBLAZE_0_AXI_INTC_MIDI_PROCESSOR_0_INT_SIG_INTR
#define MIDI_BAUD_RATE				31250

// Interrupt Controller parameters
#define INTC_DEVICE_ID				XPAR_INTC_0_DEVICE_ID
#define TIMER_INTERRUPT_ID 			XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_1_INTERRUPT_INTR

// UART output parameters
#define UART_MIDI_TX_DEVICE_ID		XPAR_AXI_UART16550_0_DEVICE_ID
#define UART_MIDI_TX_BASEADDR		XPAR_AXI_UART16550_0_BASEADDR
#define UART_MIDI_TX_CLK_FREQ		XPAR_XUARTNS550_CLOCK_HZ

// I2S Transmitter
#define I2S_TX_DEVICE_ID 			XPAR_I2S2_0_DEVICE_ID
#define I2S_TX_BASEADDR				XPAR_I2S2_0_S00_AXI_BASEADDR
#define I2S_TX_MCLK					6144000
#define I2S_TX_SAMPLE_RATE			48000
#define I2S_TX_INTERRUPT_ID			XPAR_MICROBLAZE_0_AXI_INTC_I2S2_0_READY_INTR

// Other defines

// Commonly used values
#define OFF		 0
#define ON		 1
#define CLEAR	 0
#define EMPTY	 0

// freeRTOS values
#define mainDONT_BLOCK				(portTickType) 0
#define minimum_wait				1

/************************** Variable Declarations ****************************/
// Microblaze peripheral instances
PmodOLEDrgb		 	pmodOLEDrgb_inst;			// PMOD OLED RGB screen instance
XGpio			 	GPIO_inst;					// GPIO instance
XIntc 			 	IntrptCtlrInst;				// Interrupt Controller instance
XTmrCtr			 	AXITimerInst;				// PWM timer instance
XUartNs550  	 	UART_inst;					// UART for MIDI output				// system flags

// Semaphores
xSemaphoreHandle	I2S_TX_low_sem;
xSemaphoreHandle 	I2S_TX_high_sem;
xSemaphoreHandle 	step_sem;
xSemaphoreHandle 	display_sem;
xSemaphoreHandle 	note_on_sem;
xSemaphoreHandle 	note_off_sem;
xSemaphoreHandle	mod_sem;

#endif
