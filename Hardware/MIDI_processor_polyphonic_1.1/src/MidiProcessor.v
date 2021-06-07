//////////////////////////////////////////////////////////////////////////
//	Author: Andy West (2019)
//
//	Modified for Microblaze and interrupt compatability by Mel Murphy (2021)
//////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module MidiProcessor(

    input wire clk,		                // Clock
    input wire rstn,		                // Reset
    input wire MIDI_RX,		            // midi input signal
    output wire int_sig,                // interrupt signal
    //register inputs and outputs
    output wire [63:0] note_data,
    output wire [7:0] modulation,
    output wire [1:0] interrupts,
    output wire [1:0] control_out,
    input wire [1:0] control_in
);


// Register outputs
reg note_on_reg = 0;
reg [7:0] note_reg = 8'b0;
reg [7:0] modulation_reg = 8'b0;
reg int_sig_reg;

// Interrupts and control registers
reg [1:0] interrupts_reg;
reg [1:0] control_reg;   // 0 - enable interrupts, 1 - clear interrupts


// MIDI reader signals
reg [3:0] status = 0;
reg [3:0] channel = 0;
reg [7:0] dataByte0 = 0;
reg [7:0] dataByte1 = 0;
reg [7:0] dataByte2 = 0;
reg [7:0] dataBytesReceivedCount = 0;
reg isDataByteAvailable = 0;
reg [7:0] midiNoteNumber = 0;
reg [7:0] controllerNumber = 0;

wire [7:0] note;
wire isByteAvailable;
wire [7:0] byteValue;
wire [23:0] sampleTicks;


PolyphonicProcessor poly(.clk(clk),
                    .rstn(rstn),
                    .note_in({note_on, note[6:0]}),
                    .note_data(note_data),
                    .interrupt(note_int));
                    
MidiByteReader midiByteReader(.clk(clk), .MIDI_RX(MIDI_RX), .isByteAvailable(isByteAvailable), .byteValue(byteValue));

always @(posedge clk, negedge rstn) begin
	if (~rstn) begin
		control_reg <= 2'b0;
		interrupts_reg <= 2'b0;
		int_sig_reg <= 1'b0;
	end
	else begin
		if (note_int && control_reg[0]) begin
			interrupts_reg[0] <= 1'b1;
			int_sig_reg <= 1'b1;
		end
        	if (control_in[0] == 1'b1) begin
			control_reg[0] <= 1'b1;
            	end
        	else begin
                	control_reg[0] <= 1'b0;
            	end

		if (control_in[1] == 1'b1) begin 	// clear interrupts bit 
			interrupts_reg <= 2'b0;
			int_sig_reg <= 1'b0;
	    end

        if (isByteAvailable == 1'b1) begin
                if (byteValue < 8'h80) begin	// Data Byte
                        case (dataBytesReceivedCount)
                            0: begin
                                    dataByte0 <= byteValue;
                                    dataBytesReceivedCount <= 8'd1;
                                    isDataByteAvailable <= 1'b1;
                                end
                            	1: begin
                                    	dataByte1 <= byteValue;
                                    	dataBytesReceivedCount <= 8'd2;
                                    	isDataByteAvailable <= 1'b1;
                                    end
                            	2: begin
                                	dataByte2 <= byteValue;
                                    	dataBytesReceivedCount <= 8'd3;
                                    	isDataByteAvailable <= 1'b1;
                                    end
                        	endcase
                    	end
                	else begin	// Status Byte
                        	status <= byteValue[7:4];
                        	channel <= byteValue[3:0];
                        	dataBytesReceivedCount <= 0;
                    	end
            	end
        	else if (isDataByteAvailable == 1'b1) begin

                	isDataByteAvailable <= 1'b0;

                	case (status)
                    	4'h8:  // Note Off
                        	case (dataBytesReceivedCount)
                            	1:
                               		midiNoteNumber <= dataByte0;
                            	2: begin
                                    	dataBytesReceivedCount <= 0;
                                    	note_on_reg <= 1'b0;
                                    	note_reg <= midiNoteNumber;
                                    end
                        	endcase
                    	4'h9:  // Note On
                        	case (dataBytesReceivedCount)
                            	1: midiNoteNumber <= dataByte0;
                            	2: begin
                                    	dataBytesReceivedCount <= 0;
                                    	if (dataByte1 == 0) begin
                                            // Zero velocity is like Note Off
                                            note_on_reg <= 1'b0;	
					                        note_reg <= midiNoteNumber;
                                        end
                                    	else begin
                                            note_on_reg <= 1'b1;
                                            note_reg <= midiNoteNumber;
                                        end
                                    end
                        	endcase
                    	4'hB:  // Controller Change
                        case (dataBytesReceivedCount)
                            1: controllerNumber <= dataByte0;
                            2: begin
				                    if (controllerNumber == 8'd1) begin
                                	dataBytesReceivedCount <= 0;
                                	modulation_reg <= dataByte1;
					                if(control_reg[0]) begin
                                        interrupts_reg[1] <= 1'b1;
						                  int_sig_reg <= 1'b1;
					                   end
                                 end
                             end
                        endcase
                endcase
            end
    end
end

assign interrupts = interrupts_reg;
assign control_out = control_reg;
assign int_sig = int_sig_reg;
assign note_on = note_on_reg;
assign note = note_reg;
assign modulation = modulation_reg;

endmodule
