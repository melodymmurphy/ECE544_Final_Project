`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 06/05/2021 01:25:57 AM
// Design Name: 
// Module Name: sawtriramp
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

module sawtriramp(
    input clk,
    input rstn,
    input [31:0] frequency,
    input [31:0] amplitude,
    input [31:0] wr,
    input [31:0] extra,
    output [31:0] sample
    );
    
    parameter SAMPLE_RATE = 48000;
    parameter BIT_DEPTH = 24;
    
    localparam AMP_MAX = 255;
    localparam RC_MAX = 255;
    
    integer c=0, cycle=0, amp=0;
//	wire [31:0] peak, high_cycle, low_cycle, rampUp, rampDown;
//	wire [31:0] sampleVal;

    tri_ramp tri_ramp_inst (
        // system
        .clk(clk),
        .rstn(rstn),
        // input
        .wr(wr),
        .amp_i(amp),
        .amp_max_i(AMP_MAX),
        .rise_cycle_i(extra*cycle),
        .rc_max_i(RC_MAX),
        .c_i(c_i),
        .cycle_i(cycle_i),
        // output
        .high_cycle_o(high_cycle_o),
        .sample_high_o(sample_high_o),
        .sample_low_o(sample_low_o)
    );
        
    always @(posedge clk or negedge rstn) begin
        if (!rstn) begin
            c = 0;
            cycle = 0;
            amp = 0;
//            peak = 0;
//            high_cycle = 0;
//            low_cycle = 0;
//            rampUp = 0;
//            rampDown = 0;
        end else begin
            if (wr) begin
                // calculate constants only on the beginning of a cycle t save processing power
                // reset cycle counter when at the end of a wave cycle
                cycle = (frequency > 0) ? (SAMPLE_RATE << 1 ) / frequency : 0;
                amp = (amplitude * ((1 << BIT_DEPTH) - 1));
                
                // floating point calculation
//                peak = (amplitude * ((1 << BIT_DEPTH) - 1)) / AMP_MAX;
//                high_cycle = (riseCycle * cycle) / RC_MAX;	// length of high part of duty cycle
//                low_cycle = cycle - high_cycle;
//                rampUp = peak / high_cycle;
//                rampDown = peak / low_cycle;
                
            end
            
        
            // rising side of triangle wave
//            if (c <= high_cycle) begin
//                sampleVal = c * rampUp;
//            end else begin // falling side of triangle wave
//                sampleVal = peak - (c * rampDown);
//            end
            
            // increment cycle counter
            if (c > cycle) begin
                c = 0;
            end else begin
                c = c + 1;
            end		
        end //if
    end //always

    // output nothing if frequency is zero
    // use cycle because it is latched but based on frequency
	assign sample = (cycle) ? (c <= high_cycle_o) ? sample_high_o : sample_low_o
                            : 0;

endmodule
