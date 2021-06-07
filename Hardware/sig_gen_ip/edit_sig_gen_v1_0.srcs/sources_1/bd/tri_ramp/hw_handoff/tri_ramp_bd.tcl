
################################################################
# This is a generated script based on design: tri_ramp
#
# Though there are limitations about the generated script,
# the main purpose of this utility is to make learning
# IP Integrator Tcl commands easier.
################################################################

namespace eval _tcl {
proc get_script_folder {} {
   set script_path [file normalize [info script]]
   set script_folder [file dirname $script_path]
   return $script_folder
}
}
variable script_folder
set script_folder [_tcl::get_script_folder]

################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2019.2
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   catch {common::send_msg_id "BD_TCL-109" "ERROR" "This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."}

   return 1
}

################################################################
# START
################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source tri_ramp_script.tcl

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xc7a100tcsg324-1
}


# CHANGE DESIGN NAME HERE
variable design_name
set design_name tri_ramp

# If you do not already have an existing IP Integrator design open,
# you can create a design using the following command:
#    create_bd_design $design_name

# Creating design if needed
set errMsg ""
set nRet 0

set cur_design [current_bd_design -quiet]
set list_cells [get_bd_cells -quiet]

if { ${design_name} eq "" } {
   # USE CASES:
   #    1) Design_name not set

   set errMsg "Please set the variable <design_name> to a non-empty value."
   set nRet 1

} elseif { ${cur_design} ne "" && ${list_cells} eq "" } {
   # USE CASES:
   #    2): Current design opened AND is empty AND names same.
   #    3): Current design opened AND is empty AND names diff; design_name NOT in project.
   #    4): Current design opened AND is empty AND names diff; design_name exists in project.

   if { $cur_design ne $design_name } {
      common::send_msg_id "BD_TCL-001" "INFO" "Changing value of <design_name> from <$design_name> to <$cur_design> since current design is empty."
      set design_name [get_property NAME $cur_design]
   }
   common::send_msg_id "BD_TCL-002" "INFO" "Constructing design in IPI design <$cur_design>..."

} elseif { ${cur_design} ne "" && $list_cells ne "" && $cur_design eq $design_name } {
   # USE CASES:
   #    5) Current design opened AND has components AND same names.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 1
} elseif { [get_files -quiet ${design_name}.bd] ne "" } {
   # USE CASES: 
   #    6) Current opened design, has components, but diff names, design_name exists in project.
   #    7) No opened design, design_name exists in project.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 2

} else {
   # USE CASES:
   #    8) No opened design, design_name not in project.
   #    9) Current opened design, has components, but diff names, design_name not in project.

   common::send_msg_id "BD_TCL-003" "INFO" "Currently there is no design <$design_name> in project, so creating one..."

   create_bd_design $design_name

   common::send_msg_id "BD_TCL-004" "INFO" "Making design <$design_name> as current_bd_design."
   current_bd_design $design_name

}

common::send_msg_id "BD_TCL-005" "INFO" "Currently the variable <design_name> is equal to \"$design_name\"."

if { $nRet != 0 } {
   catch {common::send_msg_id "BD_TCL-114" "ERROR" $errMsg}
   return $nRet
}

##################################################################
# DESIGN PROCs
##################################################################



# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell } {

  variable script_folder
  variable design_name

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_msg_id "BD_TCL-100" "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_msg_id "BD_TCL-101" "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create interface ports

  # Create ports
  set amp_i [ create_bd_port -dir I -from 31 -to 0 amp_i ]
  set amp_max_i [ create_bd_port -dir I -from 31 -to 0 amp_max_i ]
  set c_i [ create_bd_port -dir I -from 31 -to 0 c_i ]
  set clk [ create_bd_port -dir I -type clk clk ]
  set cycle_i [ create_bd_port -dir I -from 31 -to 0 cycle_i ]
  set high_cycle_o [ create_bd_port -dir O -from 31 -to 0 high_cycle_o ]
  set rc_max_i [ create_bd_port -dir I -from 31 -to 0 rc_max_i ]
  set rise_cycle_i [ create_bd_port -dir I -from 31 -to 0 rise_cycle_i ]
  set rstn [ create_bd_port -dir I -type rst rstn ]
  set_property -dict [ list \
   CONFIG.POLARITY {ACTIVE_HIGH} \
 ] $rstn
  set sample_high_o [ create_bd_port -dir O -from 31 -to 0 sample_high_o ]
  set sample_low_o [ create_bd_port -dir O -from 31 -to 0 sample_low_o ]
  set wr [ create_bd_port -dir I wr ]

  # Create instance: amp, and set properties
  set amp [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 amp ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {6} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Fixed_to_float} \
   CONFIG.Result_Precision_Type {Single} \
 ] $amp

  # Create instance: amp_max, and set properties
  set amp_max [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 amp_max ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {6} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Fixed_to_float} \
   CONFIG.Result_Precision_Type {Single} \
 ] $amp_max

  # Create instance: c, and set properties
  set c [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 c ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {6} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Fixed_to_float} \
   CONFIG.Result_Precision_Type {Single} \
 ] $c

  # Create instance: clk_wiz, and set properties
  set clk_wiz [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:6.0 clk_wiz ]

  # Create instance: cycle, and set properties
  set cycle [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 cycle ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {6} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Fixed_to_float} \
   CONFIG.Result_Precision_Type {Single} \
 ] $cycle

  # Create instance: high_cycle, and set properties
  set high_cycle [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 high_cycle ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {28} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Divide} \
   CONFIG.Result_Precision_Type {Single} \
 ] $high_cycle

  # Create instance: highcycle_o, and set properties
  set highcycle_o [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 highcycle_o ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {6} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {32} \
   CONFIG.C_Result_Fraction_Width {0} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Float_to_fixed} \
   CONFIG.Result_Precision_Type {Int32} \
 ] $highcycle_o

  # Create instance: low_cycle, and set properties
  set low_cycle [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 low_cycle ]
  set_property -dict [ list \
   CONFIG.Add_Sub_Value {Subtract} \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {11} \
   CONFIG.C_Mult_Usage {Full_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Add_Subtract} \
   CONFIG.Result_Precision_Type {Single} \
 ] $low_cycle

  # Create instance: peak, and set properties
  set peak [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 peak ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {28} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Divide} \
   CONFIG.Result_Precision_Type {Single} \
 ] $peak

  # Create instance: rampDown, and set properties
  set rampDown [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 rampDown ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {28} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Divide} \
   CONFIG.Result_Precision_Type {Single} \
 ] $rampDown

  # Create instance: rampUp, and set properties
  set rampUp [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 rampUp ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {28} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Divide} \
   CONFIG.Result_Precision_Type {Single} \
 ] $rampUp

  # Create instance: rc_max, and set properties
  set rc_max [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 rc_max ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {6} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Fixed_to_float} \
   CONFIG.Result_Precision_Type {Single} \
 ] $rc_max

  # Create instance: rise_cycle, and set properties
  set rise_cycle [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 rise_cycle ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {6} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Fixed_to_float} \
   CONFIG.Result_Precision_Type {Single} \
 ] $rise_cycle

  # Create instance: sample_high, and set properties
  set sample_high [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 sample_high ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {28} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Divide} \
   CONFIG.Result_Precision_Type {Single} \
 ] $sample_high

  # Create instance: sample_high_o, and set properties
  set sample_high_o [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 sample_high_o ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {6} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {32} \
   CONFIG.C_Result_Fraction_Width {0} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Float_to_fixed} \
   CONFIG.Result_Precision_Type {Int32} \
 ] $sample_high_o

  # Create instance: sample_low_mul, and set properties
  set sample_low_mul [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 sample_low_mul ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {28} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Divide} \
   CONFIG.Result_Precision_Type {Single} \
 ] $sample_low_mul

  # Create instance: sample_low_o, and set properties
  set sample_low_o [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 sample_low_o ]
  set_property -dict [ list \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {6} \
   CONFIG.C_Mult_Usage {No_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {32} \
   CONFIG.C_Result_Fraction_Width {0} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Float_to_fixed} \
   CONFIG.Result_Precision_Type {Int32} \
 ] $sample_low_o

  # Create instance: sample_low_sub, and set properties
  set sample_low_sub [ create_bd_cell -type ip -vlnv xilinx.com:ip:floating_point:7.1 sample_low_sub ]
  set_property -dict [ list \
   CONFIG.Add_Sub_Value {Subtract} \
   CONFIG.C_Accum_Input_Msb {32} \
   CONFIG.C_Accum_Lsb {-31} \
   CONFIG.C_Accum_Msb {32} \
   CONFIG.C_Latency {11} \
   CONFIG.C_Mult_Usage {Full_Usage} \
   CONFIG.C_Rate {1} \
   CONFIG.C_Result_Exponent_Width {8} \
   CONFIG.C_Result_Fraction_Width {24} \
   CONFIG.Flow_Control {NonBlocking} \
   CONFIG.Has_RESULT_TREADY {false} \
   CONFIG.Operation_Type {Add_Subtract} \
   CONFIG.Result_Precision_Type {Single} \
 ] $sample_low_sub

  # Create interface connections
  connect_bd_intf_net -intf_net amp_M_AXIS_RESULT [get_bd_intf_pins amp/M_AXIS_RESULT] [get_bd_intf_pins peak/S_AXIS_A]
  connect_bd_intf_net -intf_net amp_max_M_AXIS_RESULT [get_bd_intf_pins amp_max/M_AXIS_RESULT] [get_bd_intf_pins peak/S_AXIS_B]
  connect_bd_intf_net -intf_net c_M_AXIS_RESULT [get_bd_intf_pins c/M_AXIS_RESULT] [get_bd_intf_pins sample_high/S_AXIS_B]
  connect_bd_intf_net -intf_net cycle_M_AXIS_RESULT [get_bd_intf_pins cycle/M_AXIS_RESULT] [get_bd_intf_pins low_cycle/S_AXIS_A]
  connect_bd_intf_net -intf_net high_cycle_M_AXIS_RESULT [get_bd_intf_pins high_cycle/M_AXIS_RESULT] [get_bd_intf_pins low_cycle/S_AXIS_B]
  connect_bd_intf_net -intf_net low_cycle_M_AXIS_RESULT [get_bd_intf_pins low_cycle/M_AXIS_RESULT] [get_bd_intf_pins rampDown/S_AXIS_B]
  connect_bd_intf_net -intf_net peak_M_AXIS_RESULT [get_bd_intf_pins peak/M_AXIS_RESULT] [get_bd_intf_pins rampUp/S_AXIS_A]
  connect_bd_intf_net -intf_net rampDown_M_AXIS_RESULT [get_bd_intf_pins rampDown/M_AXIS_RESULT] [get_bd_intf_pins sample_low_mul/S_AXIS_A]
  connect_bd_intf_net -intf_net rampUp_M_AXIS_RESULT [get_bd_intf_pins rampUp/M_AXIS_RESULT] [get_bd_intf_pins sample_high/S_AXIS_A]
  connect_bd_intf_net -intf_net rc_max_M_AXIS_RESULT [get_bd_intf_pins high_cycle/S_AXIS_B] [get_bd_intf_pins rc_max/M_AXIS_RESULT]
  connect_bd_intf_net -intf_net rise_cycle_M_AXIS_RESULT [get_bd_intf_pins high_cycle/S_AXIS_A] [get_bd_intf_pins rise_cycle/M_AXIS_RESULT]
  connect_bd_intf_net -intf_net sample_high_M_AXIS_RESULT [get_bd_intf_pins sample_high/M_AXIS_RESULT] [get_bd_intf_pins sample_high_o/S_AXIS_A]
  connect_bd_intf_net -intf_net sample_low_mul_M_AXIS_RESULT [get_bd_intf_pins sample_low_mul/M_AXIS_RESULT] [get_bd_intf_pins sample_low_sub/S_AXIS_B]
  connect_bd_intf_net -intf_net sample_low_sub_M_AXIS_RESULT [get_bd_intf_pins sample_low_o/S_AXIS_A] [get_bd_intf_pins sample_low_sub/M_AXIS_RESULT]

  # Create port connections
  connect_bd_net -net c_m_axis_result_tdata [get_bd_pins c/m_axis_result_tdata] [get_bd_pins sample_low_mul/s_axis_b_tdata]
  connect_bd_net -net c_m_axis_result_tvalid [get_bd_pins c/m_axis_result_tvalid] [get_bd_pins sample_low_mul/s_axis_b_tvalid]
  connect_bd_net -net clk_in1_0_1 [get_bd_ports clk] [get_bd_pins clk_wiz/clk_in1]
  connect_bd_net -net clk_wiz_clk_out1 [get_bd_pins amp/aclk] [get_bd_pins amp_max/aclk] [get_bd_pins c/aclk] [get_bd_pins clk_wiz/clk_out1] [get_bd_pins cycle/aclk] [get_bd_pins high_cycle/aclk] [get_bd_pins highcycle_o/aclk] [get_bd_pins low_cycle/aclk] [get_bd_pins peak/aclk] [get_bd_pins rampDown/aclk] [get_bd_pins rampUp/aclk] [get_bd_pins rc_max/aclk] [get_bd_pins rise_cycle/aclk] [get_bd_pins sample_high/aclk] [get_bd_pins sample_high_o/aclk] [get_bd_pins sample_low_mul/aclk] [get_bd_pins sample_low_o/aclk] [get_bd_pins sample_low_sub/aclk]
  connect_bd_net -net high_cycle_m_axis_result_tdata [get_bd_pins high_cycle/m_axis_result_tdata] [get_bd_pins highcycle_o/s_axis_a_tdata] [get_bd_pins rampUp/s_axis_b_tdata]
  connect_bd_net -net high_cycle_m_axis_result_tvalid [get_bd_pins high_cycle/m_axis_result_tvalid] [get_bd_pins highcycle_o/s_axis_a_tvalid] [get_bd_pins rampUp/s_axis_b_tvalid]
  connect_bd_net -net highcycle_o_m_axis_result_tdata [get_bd_ports high_cycle_o] [get_bd_pins highcycle_o/m_axis_result_tdata]
  connect_bd_net -net peak_m_axis_result_tdata [get_bd_pins peak/m_axis_result_tdata] [get_bd_pins rampDown/s_axis_a_tdata] [get_bd_pins sample_low_sub/s_axis_a_tdata]
  connect_bd_net -net peak_m_axis_result_tvalid [get_bd_pins peak/m_axis_result_tvalid] [get_bd_pins rampDown/s_axis_a_tvalid] [get_bd_pins sample_low_sub/s_axis_a_tvalid]
  connect_bd_net -net reset_0_1 [get_bd_ports rstn] [get_bd_pins clk_wiz/reset]
  connect_bd_net -net s_axis_a_tdata_0_1 [get_bd_ports rc_max_i] [get_bd_pins rc_max/s_axis_a_tdata]
  connect_bd_net -net s_axis_a_tdata_1_1 [get_bd_ports c_i] [get_bd_pins c/s_axis_a_tdata]
  connect_bd_net -net s_axis_a_tdata_2_1 [get_bd_ports rise_cycle_i] [get_bd_pins rise_cycle/s_axis_a_tdata]
  connect_bd_net -net s_axis_a_tdata_3_1 [get_bd_ports amp_i] [get_bd_pins amp/s_axis_a_tdata]
  connect_bd_net -net s_axis_a_tdata_4_1 [get_bd_ports amp_max_i] [get_bd_pins amp_max/s_axis_a_tdata]
  connect_bd_net -net s_axis_a_tdata_5_1 [get_bd_ports cycle_i] [get_bd_pins cycle/s_axis_a_tdata]
  connect_bd_net -net s_axis_a_tvalid_0_1 [get_bd_ports wr] [get_bd_pins amp/s_axis_a_tvalid] [get_bd_pins amp_max/s_axis_a_tvalid] [get_bd_pins c/s_axis_a_tvalid] [get_bd_pins cycle/s_axis_a_tvalid] [get_bd_pins rc_max/s_axis_a_tvalid] [get_bd_pins rise_cycle/s_axis_a_tvalid]
  connect_bd_net -net sample_high_o_m_axis_result_tdata [get_bd_ports sample_high_o] [get_bd_pins sample_high_o/m_axis_result_tdata]
  connect_bd_net -net sample_low_o_m_axis_result_tdata [get_bd_ports sample_low_o] [get_bd_pins sample_low_o/m_axis_result_tdata]

  # Create address segments


  # Restore current instance
  current_bd_instance $oldCurInst

  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


