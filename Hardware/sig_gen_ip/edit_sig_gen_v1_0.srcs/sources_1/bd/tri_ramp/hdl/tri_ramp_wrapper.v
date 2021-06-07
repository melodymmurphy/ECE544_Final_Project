//Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2019.2 (win64) Build 2708876 Wed Nov  6 21:40:23 MST 2019
//Date        : Sun Jun  6 19:42:56 2021
//Host        : DESKTOP-J9LKVP9 running 64-bit major release  (build 9200)
//Command     : generate_target tri_ramp_wrapper.bd
//Design      : tri_ramp_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module tri_ramp_wrapper
   (amp_i,
    amp_max_i,
    c_i,
    clk,
    cycle_i,
    high_cycle_o,
    rc_max_i,
    rise_cycle_i,
    rstn,
    sample_high_o,
    sample_low_o,
    wr);
  input [31:0]amp_i;
  input [31:0]amp_max_i;
  input [31:0]c_i;
  input clk;
  input [31:0]cycle_i;
  output [31:0]high_cycle_o;
  input [31:0]rc_max_i;
  input [31:0]rise_cycle_i;
  input rstn;
  output [31:0]sample_high_o;
  output [31:0]sample_low_o;
  input wr;

  wire [31:0]amp_i;
  wire [31:0]amp_max_i;
  wire [31:0]c_i;
  wire clk;
  wire [31:0]cycle_i;
  wire [31:0]high_cycle_o;
  wire [31:0]rc_max_i;
  wire [31:0]rise_cycle_i;
  wire rstn;
  wire [31:0]sample_high_o;
  wire [31:0]sample_low_o;
  wire wr;

  tri_ramp tri_ramp_i
       (.amp_i(amp_i),
        .amp_max_i(amp_max_i),
        .c_i(c_i),
        .clk(clk),
        .cycle_i(cycle_i),
        .high_cycle_o(high_cycle_o),
        .rc_max_i(rc_max_i),
        .rise_cycle_i(rise_cycle_i),
        .rstn(rstn),
        .sample_high_o(sample_high_o),
        .sample_low_o(sample_low_o),
        .wr(wr));
endmodule
