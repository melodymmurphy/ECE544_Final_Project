`timescale 1ns / 1ps

module ByteReader_tb();

	logic clk, rstn;
	logic midiclk;

	midi_bfm bfm(
		.clk(clk),
		.midiclk(midiclk),
		.rstn(rstn)
	);
	MidiProcessor duv(
		.clk(clk),
		.rstn(rstn),
		.MIDI_RX(bfm.MIDI_RX),
		.control_in(bfm.control_in)
	);

	initial begin
		clk = 0;
		midiclk = 0;
		forever begin
			#5 clk = ~clk;
		end
	end

	initial begin
		rstn = 1;
		@(posedge clk);
		rstn = 0;
		repeat(5) @(posedge clk);
		rstn = 1;
	end

	always begin		// MIDI clock @ 31,2500 baud
		repeat(1600) @(posedge clk);
		midiclk = ~midiclk;
	end

	initial begin		// tests
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'h0, 7'h0, 7'h0);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'h0, 7'h0, 7'h0);
		repeat(10) @(posedge midiclk);

		bfm.enableInterrupts();
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h42, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h42, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h35, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h34, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h33, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h35, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h33, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h34, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.clearInterrupts();
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h01, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h02, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h03, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h04, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h05, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h06, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h07, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h08, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h09, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h0A, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h0B, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h0C, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h0D, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h0E, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h0F, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h0A, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h0F, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h0C, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h0B, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h0E, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h0D, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h06, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h08, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h09, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h01, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h07, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h69, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h42, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h69, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h69, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOn(4'hA, 7'h69, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h02, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h03, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h04, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h05, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h69, 7'h69);
		repeat(10) @(posedge midiclk);

		bfm.noteOff(4'hA, 7'h42, 7'h69);
		repeat(10) @(posedge midiclk);

		repeat(10) @(posedge midiclk);
		$stop;
	end

endmodule