`timescale 1ns / 1ps

// nexysA7fpga.v - Top level module for the Nexys A7 version of the ECE 544 Getting Started project
//
// Created By:	Roy Kravitz
// Date:		30-March-2020
// Version:		1.0
//
// Description:
// ------------
// This module provides the top level for the Getting Started hardware.
// The module assume that a PmodOLEDrgb is plugged into the JB 
// expansion ports and that a PmodENC is plugged into the JC expansion 
// port (top row).  I picked JB for the PmodOLEDrgb so that the text
// will be right side up when the switches and LEDs are on the bottom 
//////////////////////////////////////////////////////////////////////
module nexysA7fpga(
    input				clk,			// 100Mhz clock input
    input				btnC,			// center pushbutton
    input				btnU,			// UP (North) pusbhbutton
    input				btnL,			// LEFT (West) pushbutton
    input				btnD,			// DOWN (South) pushbutton  - used for system reset
    input				btnR,			// RIGHT (East) pushbutton
	input				btnCpuReset,	// CPU reset pushbutton
    input	[15:0]		sw,				// slide switches on Nexys 4
    output	[15:0] 		led,			// LEDs on Nexys 4   
    output              RGB1_Blue,      // RGB1 LED (LD16) 
    output              RGB1_Green,
    output              RGB1_Red,
    output              RGB2_Blue,      // RGB2 LED (LD17)
    output              RGB2_Green,
    output              RGB2_Red,
    output [7:0]        an,             // Seven Segment display
    output [6:0]        seg,
    output              dp,             // decimal point display on the seven segment 
    
    input				uart_rtl_rxd,	// USB UART Rx and Tx on Nexys 4
    output				uart_rtl_txd,	
    
    // PMOD I2S 
    output              mclk1,
    output              lrclk_out_t,
    output              sclk_out_t,
    output              sdata_out,
    output              mclk2,
    output              lrclk_out_r,
    output              sclk_out_r,
    input               sdata_in,

    // PMOD OLEDrgb display
    inout	[7:0] 		JB,				// JB Pmod connector - PmodOLEDrgb
    
    // PMOD rotary encoder
    input               encA,
    input               encB,
    input               encBTN,
    input               encSWT,
    
    // MIDI input and output
	input               midi_in,
	output              midi_out
);

// internal variables
// Clock and Reset 
wire				sysclk;
wire				sysreset_n, sysreset;

// OLED pins 
wire 				pmodoledrgb_out_pin1_i, pmodoledrgb_out_pin1_io, pmodoledrgb_out_pin1_o, pmodoledrgb_out_pin1_t; 
wire 				pmodoledrgb_out_pin2_i, pmodoledrgb_out_pin2_io, pmodoledrgb_out_pin2_o, pmodoledrgb_out_pin2_t; 
wire 				pmodoledrgb_out_pin3_i, pmodoledrgb_out_pin3_io, pmodoledrgb_out_pin3_o, pmodoledrgb_out_pin3_t; 
wire 				pmodoledrgb_out_pin4_i, pmodoledrgb_out_pin4_io, pmodoledrgb_out_pin4_o, pmodoledrgb_out_pin4_t; 
wire 				pmodoledrgb_out_pin7_i, pmodoledrgb_out_pin7_io, pmodoledrgb_out_pin7_o, pmodoledrgb_out_pin7_t; 
wire 				pmodoledrgb_out_pin8_i, pmodoledrgb_out_pin8_io, pmodoledrgb_out_pin8_o, pmodoledrgb_out_pin8_t; 
wire 				pmodoledrgb_out_pin9_i, pmodoledrgb_out_pin9_io, pmodoledrgb_out_pin9_o, pmodoledrgb_out_pin9_t; 
wire 				pmodoledrgb_out_pin10_i, pmodoledrgb_out_pin10_io, pmodoledrgb_out_pin10_o, pmodoledrgb_out_pin10_t;

// RGB LED 
wire                w_RGB1_Red, w_RGB1_Blue, w_RGB1_Green;

// LED pins 
wire    [15:0]      led_int;                // LED outputs

// Drive the leds from the signal generated by the microblaze 
assign led = led_int;                   // LEDs are driven by led

// I2s connections
wire lrclk_out;
wire sclk_out;
wire mclk_out;

// Debounced button and switch signals

wire btnCpuReset_db;
wire btnC_db;
wire btnU_db;
wire btnL_db;
wire btnD_db;
wire btnR_db;
wire [15:0] sw_db;

assign mclk1 = mclk_out;
assign mclk2 = mclk_out;
assign lrclk_out_r = lrclk_out;
assign lrclk_out_t = lrclk_out;
assign sclk_out_r = sclk_out;
assign sclk_out_t = sclk_out;
    
// make the connections
// system-wide signals
assign sysclk = clk;
assign sysreset_n = btnCpuReset_db;		// The CPU reset pushbutton is asserted low.  The other pushbuttons are asserted high
										// but the Microblaze for Nexys 4 expects reset to be asserted low
assign sysreset = ~sysreset_n;			// Generate a reset signal that is asserted high for any logic blocks expecting it.

// Pmod OLED connections 
assign JB[0] = pmodoledrgb_out_pin1_io;
assign JB[1] = pmodoledrgb_out_pin2_io;
assign JB[2] = pmodoledrgb_out_pin3_io;
assign JB[3] = pmodoledrgb_out_pin4_io;
assign JB[4] = pmodoledrgb_out_pin7_io;
assign JB[5] = pmodoledrgb_out_pin8_io;
assign JB[6] = pmodoledrgb_out_pin9_io;
assign JB[7] = pmodoledrgb_out_pin10_io;

// instantiate the embedded system
embsys EMBSYS
       (// PMOD OLED pins 
        .PmodOLEDrgb_out_0_pin10_i(pmodoledrgb_out_pin10_i),
	    .PmodOLEDrgb_out_0_pin10_o(pmodoledrgb_out_pin10_o),
	    .PmodOLEDrgb_out_0_pin10_t(pmodoledrgb_out_pin10_t),
	    .PmodOLEDrgb_out_0_pin1_i(pmodoledrgb_out_pin1_i),
	    .PmodOLEDrgb_out_0_pin1_o(pmodoledrgb_out_pin1_o),
	    .PmodOLEDrgb_out_0_pin1_t(pmodoledrgb_out_pin1_t),
	    .PmodOLEDrgb_out_0_pin2_i(pmodoledrgb_out_pin2_i),
	    .PmodOLEDrgb_out_0_pin2_o(pmodoledrgb_out_pin2_o),
	    .PmodOLEDrgb_out_0_pin2_t(pmodoledrgb_out_pin2_t),
	    .PmodOLEDrgb_out_0_pin3_i(pmodoledrgb_out_pin3_i),
	    .PmodOLEDrgb_out_0_pin3_o(pmodoledrgb_out_pin3_o),
	    .PmodOLEDrgb_out_0_pin3_t(pmodoledrgb_out_pin3_t),
	    .PmodOLEDrgb_out_0_pin4_i(pmodoledrgb_out_pin4_i),
	    .PmodOLEDrgb_out_0_pin4_o(pmodoledrgb_out_pin4_o),
	    .PmodOLEDrgb_out_0_pin4_t(pmodoledrgb_out_pin4_t),
	    .PmodOLEDrgb_out_0_pin7_i(pmodoledrgb_out_pin7_i),
	    .PmodOLEDrgb_out_0_pin7_o(pmodoledrgb_out_pin7_o),
	    .PmodOLEDrgb_out_0_pin7_t(pmodoledrgb_out_pin7_t),
	    .PmodOLEDrgb_out_0_pin8_i(pmodoledrgb_out_pin8_i),
	    .PmodOLEDrgb_out_0_pin8_o(pmodoledrgb_out_pin8_o),
	    .PmodOLEDrgb_out_0_pin8_t(pmodoledrgb_out_pin8_t),
	    .PmodOLEDrgb_out_0_pin9_i(pmodoledrgb_out_pin9_i),
	    .PmodOLEDrgb_out_0_pin9_o(pmodoledrgb_out_pin9_o),
	    .PmodOLEDrgb_out_0_pin9_t(pmodoledrgb_out_pin9_t),
        // PMOD encoder pins
        .encA_0(encA),
        .encB_0(encB),
        .encBTN_0(encBTN),
        .encSWT_0(encSWT),
        // MIDI input
        .midi_in_0(midi_in),
        // UART for MIDI output
        .midi_out_0(midi_out),
        // I2S signals
        .mclk_0(mclk_out),
        .lrclk_0(lrclk_out),
        .sclk_0(sclk_out),
        .sdout_0(sdata_out),
        // RGB1/2 Led's 
        .RGB1_Blue_0(RGB1_Blue),
        .RGB1_Green_0(RGB1_Green),
        .RGB1_Red_0(RGB1_Red),
        .RGB2_Blue_0(RGB2_Blue),
        .RGB2_Green_0(RGB2_Green),
        .RGB2_Red_0(RGB2_Red),
        // Seven Segment Display anode control  
        .an_0(an),
        .dp_0(dp),
        .seg_0(seg),
        // LEDs
        .leds_0_tri_o(led_int),
        // Push buttons and switches  
        .btnC_0(btnC_db),
        .btnD_0(btnD_db),
        .btnL_0(btnL_db),
        .btnR_0(btnR_db),
        .btnU_0(btnU_db),
        .sw_0(sw_db),
        // reset and clock 
        .sysreset_n(sysreset_n),
        .sysclk(sysclk),
        // UART pins 
        .usb_uart_rxd(uart_rtl_rxd),
        .usb_uart_txd(uart_rtl_txd));
        
// Tristate buffers for the pmodOLEDrgb pins
// generated by PMOD bridge component.  Many
// of these signals are not tri-state.
IOBUF pmodoledrgb_out_pin1_iobuf
(
    .I(pmodoledrgb_out_pin1_o),
    .IO(pmodoledrgb_out_pin1_io),
    .O(pmodoledrgb_out_pin1_i),
    .T(pmodoledrgb_out_pin1_t)
);

IOBUF pmodoledrgb_out_pin2_iobuf
(
    .I(pmodoledrgb_out_pin2_o),
    .IO(pmodoledrgb_out_pin2_io),
    .O(pmodoledrgb_out_pin2_i),
    .T(pmodoledrgb_out_pin2_t)
);

IOBUF pmodoledrgb_out_pin3_iobuf
(
    .I(pmodoledrgb_out_pin3_o),
    .IO(pmodoledrgb_out_pin3_io),
    .O(pmodoledrgb_out_pin3_i),
    .T(pmodoledrgb_out_pin3_t)
);

IOBUF pmodoledrgb_out_pin4_iobuf
(
    .I(pmodoledrgb_out_pin4_o),
    .IO(pmodoledrgb_out_pin4_io),
    .O(pmodoledrgb_out_pin4_i),
    .T(pmodoledrgb_out_pin4_t)
);

IOBUF pmodoledrgb_out_pin7_iobuf
(
    .I(pmodoledrgb_out_pin7_o),
    .IO(pmodoledrgb_out_pin7_io),
    .O(pmodoledrgb_out_pin7_i),
    .T(pmodoledrgb_out_pin7_t)
);

IOBUF pmodoledrgb_out_pin8_iobuf
(
    .I(pmodoledrgb_out_pin8_o),
    .IO(pmodoledrgb_out_pin8_io),
    .O(pmodoledrgb_out_pin8_i),
    .T(pmodoledrgb_out_pin8_t)
);

IOBUF pmodoledrgb_out_pin9_iobuf
(
    .I(pmodoledrgb_out_pin9_o),
    .IO(pmodoledrgb_out_pin9_io),
    .O(pmodoledrgb_out_pin9_i),
    .T(pmodoledrgb_out_pin9_t)
);

IOBUF pmodoledrgb_out_pin10_iobuf
(
    .I(pmodoledrgb_out_pin10_o),
    .IO(pmodoledrgb_out_pin10_io),
    .O(pmodoledrgb_out_pin10_i),
    .T(pmodoledrgb_out_pin10_t)
);

debounce debouncer(
	.clk(clk),
	.pbtn_in({btnU, btnL, btnR, btnD, btnC, btnCpuReset}),
	.switch_in(sw),
	.pbtn_db({btnU_db, btnL_db, btnR_db, btnD_db, btnC_db, btnCpuReset_db}),
	.swtch_db(sw_db)
);

endmodule

