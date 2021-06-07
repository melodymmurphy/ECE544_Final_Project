//Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2019.2 (win64) Build 2708876 Wed Nov  6 21:40:23 MST 2019
//Date        : Sun Jun  6 19:42:56 2021
//Host        : DESKTOP-J9LKVP9 running 64-bit major release  (build 9200)
//Command     : generate_target tri_ramp.bd
//Design      : tri_ramp
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CORE_GENERATION_INFO = "tri_ramp,IP_Integrator,{x_ipVendor=xilinx.com,x_ipLibrary=BlockDiagram,x_ipName=tri_ramp,x_ipVersion=1.00.a,x_ipLanguage=VERILOG,numBlks=18,numReposBlks=18,numNonXlnxBlks=0,numHierBlks=0,maxHierDepth=0,numSysgenBlks=0,numHlsBlks=0,numHdlrefBlks=0,numPkgbdBlks=0,bdsource=USER,da_clkrst_cnt=3,synth_mode=OOC_per_IP}" *) (* HW_HANDOFF = "tri_ramp.hwdef" *) 
module tri_ramp
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
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 CLK.CLK CLK" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME CLK.CLK, CLK_DOMAIN tri_ramp_clk_in1_0, FREQ_HZ 100000000, INSERT_VIP 0, PHASE 0.000" *) input clk;
  input [31:0]cycle_i;
  output [31:0]high_cycle_o;
  input [31:0]rc_max_i;
  input [31:0]rise_cycle_i;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 RST.RSTN RST" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME RST.RSTN, INSERT_VIP 0, POLARITY ACTIVE_HIGH" *) input rstn;
  output [31:0]sample_high_o;
  output [31:0]sample_low_o;
  input wr;

  wire [31:0]amp_M_AXIS_RESULT_TDATA;
  wire amp_M_AXIS_RESULT_TVALID;
  wire [31:0]amp_max_M_AXIS_RESULT_TDATA;
  wire amp_max_M_AXIS_RESULT_TVALID;
  wire [31:0]c_m_axis_result_tdata;
  wire c_m_axis_result_tvalid;
  wire clk_in1_0_1;
  wire clk_wiz_clk_out1;
  wire [31:0]cycle_M_AXIS_RESULT_TDATA;
  wire cycle_M_AXIS_RESULT_TVALID;
  wire [31:0]high_cycle_m_axis_result_tdata;
  wire high_cycle_m_axis_result_tvalid;
  wire [31:0]highcycle_o_m_axis_result_tdata;
  wire [31:0]low_cycle_M_AXIS_RESULT_TDATA;
  wire low_cycle_M_AXIS_RESULT_TVALID;
  wire [31:0]peak_m_axis_result_tdata;
  wire peak_m_axis_result_tvalid;
  wire [31:0]rampDown_M_AXIS_RESULT_TDATA;
  wire rampDown_M_AXIS_RESULT_TVALID;
  wire [31:0]rampUp_M_AXIS_RESULT_TDATA;
  wire rampUp_M_AXIS_RESULT_TVALID;
  wire [31:0]rc_max_M_AXIS_RESULT_TDATA;
  wire rc_max_M_AXIS_RESULT_TVALID;
  wire reset_0_1;
  wire [31:0]rise_cycle_M_AXIS_RESULT_TDATA;
  wire rise_cycle_M_AXIS_RESULT_TVALID;
  wire [31:0]s_axis_a_tdata_0_1;
  wire [31:0]s_axis_a_tdata_1_1;
  wire [31:0]s_axis_a_tdata_2_1;
  wire [31:0]s_axis_a_tdata_3_1;
  wire [31:0]s_axis_a_tdata_4_1;
  wire [31:0]s_axis_a_tdata_5_1;
  wire s_axis_a_tvalid_0_1;
  wire [31:0]sample_high_M_AXIS_RESULT_TDATA;
  wire sample_high_M_AXIS_RESULT_TVALID;
  wire [31:0]sample_high_o_m_axis_result_tdata;
  wire [31:0]sample_low_mul_M_AXIS_RESULT_TDATA;
  wire sample_low_mul_M_AXIS_RESULT_TVALID;
  wire [31:0]sample_low_o_m_axis_result_tdata;
  wire [31:0]sample_low_sub_M_AXIS_RESULT_TDATA;
  wire sample_low_sub_M_AXIS_RESULT_TVALID;

  assign clk_in1_0_1 = clk;
  assign high_cycle_o[31:0] = highcycle_o_m_axis_result_tdata;
  assign reset_0_1 = rstn;
  assign s_axis_a_tdata_0_1 = rc_max_i[31:0];
  assign s_axis_a_tdata_1_1 = c_i[31:0];
  assign s_axis_a_tdata_2_1 = rise_cycle_i[31:0];
  assign s_axis_a_tdata_3_1 = amp_i[31:0];
  assign s_axis_a_tdata_4_1 = amp_max_i[31:0];
  assign s_axis_a_tdata_5_1 = cycle_i[31:0];
  assign s_axis_a_tvalid_0_1 = wr;
  assign sample_high_o[31:0] = sample_high_o_m_axis_result_tdata;
  assign sample_low_o[31:0] = sample_low_o_m_axis_result_tdata;
  tri_ramp_floating_point_0_0 amp
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(amp_M_AXIS_RESULT_TDATA),
        .m_axis_result_tvalid(amp_M_AXIS_RESULT_TVALID),
        .s_axis_a_tdata(s_axis_a_tdata_3_1),
        .s_axis_a_tvalid(s_axis_a_tvalid_0_1));
  tri_ramp_floating_point_1_0 amp_max
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(amp_max_M_AXIS_RESULT_TDATA),
        .m_axis_result_tvalid(amp_max_M_AXIS_RESULT_TVALID),
        .s_axis_a_tdata(s_axis_a_tdata_4_1),
        .s_axis_a_tvalid(s_axis_a_tvalid_0_1));
  tri_ramp_floating_point_3_0 c
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(c_m_axis_result_tdata),
        .m_axis_result_tvalid(c_m_axis_result_tvalid),
        .s_axis_a_tdata(s_axis_a_tdata_1_1),
        .s_axis_a_tvalid(s_axis_a_tvalid_0_1));
  tri_ramp_clk_wiz_0 clk_wiz
       (.clk_in1(clk_in1_0_1),
        .clk_out1(clk_wiz_clk_out1),
        .reset(reset_0_1));
  tri_ramp_floating_point_1_1 cycle
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(cycle_M_AXIS_RESULT_TDATA),
        .m_axis_result_tvalid(cycle_M_AXIS_RESULT_TVALID),
        .s_axis_a_tdata(s_axis_a_tdata_5_1),
        .s_axis_a_tvalid(s_axis_a_tvalid_0_1));
  tri_ramp_floating_point_7_0 high_cycle
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(high_cycle_m_axis_result_tdata),
        .m_axis_result_tvalid(high_cycle_m_axis_result_tvalid),
        .s_axis_a_tdata(rise_cycle_M_AXIS_RESULT_TDATA),
        .s_axis_a_tvalid(rise_cycle_M_AXIS_RESULT_TVALID),
        .s_axis_b_tdata(rc_max_M_AXIS_RESULT_TDATA),
        .s_axis_b_tvalid(rc_max_M_AXIS_RESULT_TVALID));
  tri_ramp_mul_1 highcycle_o
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(highcycle_o_m_axis_result_tdata),
        .s_axis_a_tdata(high_cycle_m_axis_result_tdata),
        .s_axis_a_tvalid(high_cycle_m_axis_result_tvalid));
  tri_ramp_sub_0 low_cycle
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(low_cycle_M_AXIS_RESULT_TDATA),
        .m_axis_result_tvalid(low_cycle_M_AXIS_RESULT_TVALID),
        .s_axis_a_tdata(cycle_M_AXIS_RESULT_TDATA),
        .s_axis_a_tvalid(cycle_M_AXIS_RESULT_TVALID),
        .s_axis_b_tdata({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axis_b_tvalid(1'b0));
  tri_ramp_floating_point_0_1 peak
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(peak_m_axis_result_tdata),
        .m_axis_result_tvalid(peak_m_axis_result_tvalid),
        .s_axis_a_tdata(amp_M_AXIS_RESULT_TDATA),
        .s_axis_a_tvalid(amp_M_AXIS_RESULT_TVALID),
        .s_axis_b_tdata(amp_max_M_AXIS_RESULT_TDATA),
        .s_axis_b_tvalid(amp_max_M_AXIS_RESULT_TVALID));
  tri_ramp_floating_point_1_4 rampDown
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(rampDown_M_AXIS_RESULT_TDATA),
        .m_axis_result_tvalid(rampDown_M_AXIS_RESULT_TVALID),
        .s_axis_a_tdata(peak_m_axis_result_tdata),
        .s_axis_a_tvalid(peak_m_axis_result_tvalid),
        .s_axis_b_tdata(low_cycle_M_AXIS_RESULT_TDATA),
        .s_axis_b_tvalid(low_cycle_M_AXIS_RESULT_TVALID));
  tri_ramp_floating_point_7_1 rampUp
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(rampUp_M_AXIS_RESULT_TDATA),
        .m_axis_result_tvalid(rampUp_M_AXIS_RESULT_TVALID),
        .s_axis_a_tdata({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axis_a_tvalid(1'b0),
        .s_axis_b_tdata(high_cycle_m_axis_result_tdata),
        .s_axis_b_tvalid(high_cycle_m_axis_result_tvalid));
  tri_ramp_floating_point_1_2 rc_max
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(rc_max_M_AXIS_RESULT_TDATA),
        .m_axis_result_tvalid(rc_max_M_AXIS_RESULT_TVALID),
        .s_axis_a_tdata(s_axis_a_tdata_0_1),
        .s_axis_a_tvalid(s_axis_a_tvalid_0_1));
  tri_ramp_floating_point_0_2 rise_cycle
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(rise_cycle_M_AXIS_RESULT_TDATA),
        .m_axis_result_tvalid(rise_cycle_M_AXIS_RESULT_TVALID),
        .s_axis_a_tdata(s_axis_a_tdata_2_1),
        .s_axis_a_tvalid(s_axis_a_tvalid_0_1));
  tri_ramp_mul_0 sample_high
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(sample_high_M_AXIS_RESULT_TDATA),
        .m_axis_result_tvalid(sample_high_M_AXIS_RESULT_TVALID),
        .s_axis_a_tdata(rampUp_M_AXIS_RESULT_TDATA),
        .s_axis_a_tvalid(rampUp_M_AXIS_RESULT_TVALID),
        .s_axis_b_tdata({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axis_b_tvalid(1'b0));
  tri_ramp_sout_0 sample_high_o_RnM
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(sample_high_o_m_axis_result_tdata),
        .s_axis_a_tdata(sample_high_M_AXIS_RESULT_TDATA),
        .s_axis_a_tvalid(sample_high_M_AXIS_RESULT_TVALID));
  tri_ramp_floating_point_8_0 sample_low_mul
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(sample_low_mul_M_AXIS_RESULT_TDATA),
        .m_axis_result_tvalid(sample_low_mul_M_AXIS_RESULT_TVALID),
        .s_axis_a_tdata(rampDown_M_AXIS_RESULT_TDATA),
        .s_axis_a_tvalid(rampDown_M_AXIS_RESULT_TVALID),
        .s_axis_b_tdata(c_m_axis_result_tdata),
        .s_axis_b_tvalid(c_m_axis_result_tvalid));
  tri_ramp_sout_1 sample_low_o_RnM
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(sample_low_o_m_axis_result_tdata),
        .s_axis_a_tdata(sample_low_sub_M_AXIS_RESULT_TDATA),
        .s_axis_a_tvalid(sample_low_sub_M_AXIS_RESULT_TVALID));
  tri_ramp_floating_point_1_3 sample_low_sub
       (.aclk(clk_wiz_clk_out1),
        .m_axis_result_tdata(sample_low_sub_M_AXIS_RESULT_TDATA),
        .m_axis_result_tvalid(sample_low_sub_M_AXIS_RESULT_TVALID),
        .s_axis_a_tdata(peak_m_axis_result_tdata),
        .s_axis_a_tvalid(peak_m_axis_result_tvalid),
        .s_axis_b_tdata(sample_low_mul_M_AXIS_RESULT_TDATA),
        .s_axis_b_tvalid(sample_low_mul_M_AXIS_RESULT_TVALID));
endmodule
