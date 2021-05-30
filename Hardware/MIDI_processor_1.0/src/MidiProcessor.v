//////////////////////////////////////////////////////////////////////////
//	Author: Andy West (2019)
//
//	Modified for RVFPGA and Wishbone compatability by Mel Murphy (2021)
//////////////////////////////////////////////////////////////////////////



module MidiProcessor(

    input wire clk,		                // Clock
    input wire rst,		                // Reset
    input wire MIDI_RX,		            // midi input signal
    output wire int_sig,                // interrupt signal
    //register inputs and outputs
    output wire note_on,
    output wire [7:0] note,
    output wire [7:0] modulation,
    output wire [7:0] interrupts_out,
    input wire [3:0] interrupts_in,
    output wire [1:0] control_out,
    input wire [31:0] control_in
);


// Register outputs
reg note_on_reg = 0;
reg [7:0] note_reg = 8'b0;
reg [7:0] modulation_reg = 8'b0;

// Interrupt and control registers
reg [3:0] interrupt_reg;   // 0 - active interrupt, 1 - note on, 2 - note off, 3 - modulation
reg [1:0] control_reg;   // 0 - enable interrupts, 1 - clear interrupts

// Interrupt signal
reg int_sig_reg;

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

wire isByteAvailable;
wire [7:0] byteValue;
wire [23:0] sampleTicks;



MidiByteReader midiByteReader(.clk(clk), .MIDI_RX(MIDI_RX), .isByteAvailable(isByteAvailable), .byteValue(byteValue));

always @(posedge clk, posedge rst) begin
	if (~rst) begin
		interrupt_reg <= 4'b0;
		control_reg <= 2'b0;
		int_sig_reg <= 0;
	end
	else begin

        if (control_in[0] == 1'b1)
            begin
                control_reg[0] <= 1'b1;
            end
        else
            begin
                control_reg[0] <= 1'b0;
            end
        if (control_in[1] == 1'b1) // interrupt clear bit
            begin
                interrupt_reg <= 4'b0;
                int_sig_reg <= 1'b0;
            end
        else
        	begin
        		interrupt_reg <= interrupt_reg;
        		int_sig_reg <= int_sig_reg;
        	end


        if (isByteAvailable == 1'b1)
            begin
                if (byteValue < 8'h80)  // Data byte
                    begin
                        case (dataBytesReceivedCount)
                            0:
                                begin
                                    dataByte0 <= byteValue;
                                    dataBytesReceivedCount <= 8'd1;
                                    isDataByteAvailable <= 1'b1;
                                end
                            1:
                                begin
                                    dataByte1 <= byteValue;
                                    dataBytesReceivedCount <= 8'd2;
                                    isDataByteAvailable <= 1'b1;
                                end
                            2:
                                begin
                                    dataByte2 <= byteValue;
                                    dataBytesReceivedCount <= 8'd3;
                                    isDataByteAvailable <= 1'b1;
                                end
                        endcase
                    end
                else  // Status byte
                    begin
                        status <= byteValue[7:4];
                        channel <= byteValue[3:0];
                        dataBytesReceivedCount <= 0;
                    end
            end
        else if (isDataByteAvailable == 1'b1)
            begin
                isDataByteAvailable <= 1'b0;
            
                case (status)
                    4'h8:  // Note Off
                        if (dataBytesReceivedCount == 2)
                            begin
                                if (midiNoteNumber == dataByte0)
                                    begin
                                        dataBytesReceivedCount <= 0;
                                        note_on_reg <= 1'b0;
                                        if (control_reg[0]) begin
                                            int_sig_reg  <= 1'b1;
                                            interrupt_reg[2] <= 1'b1;
                                            interrupt_reg[0] <= 1'b1;
                                        end
                                    end
                            end
                    4'h9:  // Note On
                        case (dataBytesReceivedCount)
                            1:
                                midiNoteNumber <= dataByte0;
                            2:
                                begin
                                    dataBytesReceivedCount <= 0;
    
                                    if (dataByte1 == 0)
                                        begin
                                            // Zero velocity is like Note Off
                                            note_on_reg <= 1'b0;	
                                            if (control_reg[0]) begin
                                                int_sig_reg  <= 1'b1;	
                                                interrupt_reg[2] <= 1'b1;		
                                                interrupt_reg[0] <= 1'b1;
                                            end
                                        end
                                    else
                                        begin
                                            note_on_reg <= 1'b1;
                                            note_reg <= midiNoteNumber;
                                            if (control_reg[0]) begin
                                                int_sig_reg  <= 1'b1;
                                                interrupt_reg[1] <= 1'b1;
                                                interrupt_reg[0] <= 1'b1;
                                            end
                                        end
                                end
                        endcase
                    4'hB:  // Controller Change
                        case (dataBytesReceivedCount)
                            1:
                                controllerNumber <= dataByte0;
                            2:
                                begin
                                    if (controllerNumber == 8'd1)
                                        begin
                                            dataBytesReceivedCount <= 0;
                                            modulation_reg <= dataByte1;
                                            if (control_reg[0]) begin
                                                int_sig_reg  <= 1'b1;
                                                interrupt_reg[3] <= 1'b1;
                                                interrupt_reg[0] <= 1'b1;
                                            end
                                        end
                                end
                        endcase
                endcase
            end
    end
end

assign interrupts_out = interrupt_reg;
assign control_out = control_reg;
assign int_sig = int_sig_reg;
assign note_on = note_on_reg;
assign note = note_reg;
assign modulation = modulation_reg;

endmodule
