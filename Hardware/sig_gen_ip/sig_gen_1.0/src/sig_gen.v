`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 06/06/2021 12:52:48 AM
// Design Name: 
// Module Name: sig_gen
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


module sig_gen
    (
    input clk,
    input rstn,
    
    input [31:0] frequency,
    input [31:0] amplitude,
    input [31:0] extra,         // riseCycle, 
    input [31:0] mask,          // mask of selected wave gen
    output [31:0] sample
    );
    
    parameter WAVE_CNT = 4;
    
    wire [31:0] wr;
    wire [31:0] wave_sample [WAVE_CNT-1:0];
    
    genvar i;
    generate 
        for (i=0; i<WAVE_CNT; i=i+1) begin
            assign wr = mask&(1<<i);
            sawtriramp sawtriramp(
                .clk        (clk),
                .rstn       (rstn),
                .frequency  (frequency),
                .amplitude  (amplitude),
                .extra      (extra),
                .wr         (wr),
                .sample     (wave_sample[i])
            );
        end
    endgenerate
    
    // mixer
    integer j, active=0;
    integer wave_active=0, wave_num=0;
    integer sample;
    always @(posedge clk or negedge rstn) begin
        if (!rstn) begin
            active = 0;
            wave_active = 0;
            wave_num = 0;
            sample = 0;
        end else begin
            // active generators
            if (mask & frequency) begin
                wave_active = wave_active | mask;
            end else begin
                wave_active = wave_active & ~mask;
            end
            
            // average samples of active generators
            wave_num = 0;
            for (j=0; j<(WAVE_CNT*3); j=j+1) begin
                active = wave_active&(1<<j);
                if (active) begin
                    wave_num = wave_num + 1;
                    sample = sample + wave_sample[i];
                end
            end
            sample = sample / wave_num;
        end
    end
    
endmodule
