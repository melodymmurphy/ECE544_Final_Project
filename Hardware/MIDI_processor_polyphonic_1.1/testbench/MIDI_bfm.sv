`timescale 1ns / 1ps

interface midi_bfm(input logic clk, input logic midiclk, input logic rstn);

	reg MIDI_RX = 1'b1;		// midi input signal
	logic [1:0] control_in = 2'b0;

	task writeSignal(logic [7:0] byteVal);
		MIDI_RX <= 0;
		@(posedge midiclk);
		for (int i=0; i < 8; i++)
			begin
				MIDI_RX <= byteVal[i];
				@(posedge midiclk);
			end
		MIDI_RX <= 1;
		@(posedge midiclk);
	endtask

	task noteOn(logic [3:0] channel, logic [6:0] note, logic [6:0] velocity);
		writeSignal({4'h9,channel});
		writeSignal({1'b0,note});
		writeSignal({1'b0,velocity});
	endtask

	task noteOff(logic [3:0] channel, logic [6:0] note, logic [6:0] velocity);
		writeSignal({4'h8,channel});
		writeSignal({1'b0,note});
		writeSignal({1'b0,velocity});
	endtask

	task enableInterrupts();
		control_in[0] = 1'b1;
		@(posedge clk);
	endtask;

	task clearInterrupts();
		control_in[1] = 1'b1;
		@(posedge clk);
		control_in[1] = 1'b0;
		@(posedge clk);
	endtask;

	always begin
		@(posedge clk);

	end

endinterface