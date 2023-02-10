`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// PolyphonicProcessor.v
//
// Melody Murphy
// Portland State University
// ECE 544 Final Project
// Spring 2021 
//
// This module reads NOTE ON and NOTE OFF MIDI messages and stores the
// currently active notes to a register that's accessible by external
// modules. When the register state changes, it sends an edge-triggered
// interrupt.
//
//////////////////////////////////////////////////////////////////////////////////


module PolyphonicProcessor
(
	input wire clk,
	input wire rstn,
	input wire [7:0] note_in,
	output wire [63:0] note_data,
	output logic interrupt
);

	logic [6:0] notes [7:0];
	logic [7:0] current_note;
	logic [2:0] current_index;
	logic [2:0] open_index;

	// encoding of machine states
	typedef enum logic [11:0] {
		WAIT		= 12'b000000000001,
		FETCH		= 12'b000000000010,		// one-hot encoding
		C0			= 12'b000000000100,
		C1			= 12'b000000001000,
		C2			= 12'b000000010000,
		C3			= 12'b000000100000,
		C4			= 12'b000001000000,
		C5			= 12'b000010000000,
		C6			= 12'b000100000000,
		C7			= 12'b001000000000,
		ADD			= 12'b010000000000,
		SUB			= 12'b100000000000
	} states_t;

	states_t state, next_state;	// variables to hold current and next state

	// state sequencer
	always_ff @(posedge clk, negedge rstn) begin : state_sequencer
		if (~rstn) begin
			state 								<= WAIT;
			current_note 						<= 0;
			current_index 						<= 0;
			open_index 							<= 0;
			for (integer i = 0; i < 8; i++)
				notes[i] 						<= 0;
		end
		else begin
			state 								<= next_state;
			current_note 						<= current_note;
			current_index 						<= current_index;
			open_index 							<= open_index;
			notes 								<= notes;
			unique case (1'b1)
				state[ 0]:  ;
				state[ 1]:  current_note 		<= note_in;
				state[ 2]:  begin
								current_index 	<= 0;
								if (notes[0] == 0)
									open_index 	<= 0;
					end
				state[ 3]:  begin
								current_index 	<= 1;
								if (notes[1] == 0)
									open_index 	<= 1;
					end
				state[ 4]:  begin
								current_index 	<= 2;
								if (notes[2] == 0)
									open_index 	<= 2;
					end
				state[ 5]:  begin
								current_index	<= 3;
								if (notes[3] == 0)
									open_index 	<= 3;
					end
				state[ 6]:  begin
								current_index	<= 4;
								if (notes[4] == 0)
									open_index 	<= 4;
					end
				state[ 7]:  begin
								current_index 	<= 5;
								if (notes[5] == 0)
									open_index 	<= 5;
					end
				state[ 8]:  begin
								current_index 	<= 6;
								if (notes[6] == 0)
									open_index 	<= 6;
					end
				state[ 9]:  begin
								current_index 	<= 7;
								if (notes[7] == 0)
									open_index 	<= 7;
					end
				state[10]: notes[open_index] 	<= current_note;	// add to active notes
				state[11]: notes[current_index] <= 0;		// remove from active notes
			endcase
		end //else
	end : state_sequencer


	// next state decoder
	always_comb begin : next_state_decoder
		if (~rstn)
			next_state = WAIT;
		else begin
			unique case (1'b1)
				state[0]:   begin	// WAIT
								if (note_in != current_note)	
									next_state = FETCH;
								else
									next_state = WAIT;
				   			end
				state[1]:   begin	// FETCH
								next_state = C0;
				   			end
				state[2]:   begin	// C0
								if (current_note == notes[0]) begin
									if (current_note[7])
										next_state = WAIT;
									else
										next_state = SUB;
								end
								else
									next_state = C1;
							end
				state[3]:   begin	// C1
								if (current_note == notes[1]) begin
									if (current_note[7])
										next_state = WAIT;
									else
										next_state = SUB;
								end
								else
									next_state = C2;
							end
				state[4]:   begin	// C2
								if (current_note == notes[2]) begin
									if (current_note[7])
										next_state = WAIT;
									else
										next_state = SUB;
								end
								else
									next_state = C3;
							end
				state[5]:   begin	// C3
								if (current_note == notes[3]) begin
									if (current_note[7])
										next_state = WAIT;
									else
										next_state = SUB;
								end
								else
									next_state = C4;
							end
				state[6]:   begin	// C4
								if (current_note == notes[4]) begin
									if (current_note[7])
										next_state = WAIT;
									else
										next_state = SUB;
								end
								else
									next_state = C5;
							end
				state[7]:   begin	// C5
								if (current_note == notes[5]) begin
									if (current_note[7])
										next_state = WAIT;
									else
										next_state = SUB;
								end
								else
									next_state = C6;
							end
				state[8]:   begin	// C6
								if (current_note == notes[6]) begin
									if (current_note[7])
										next_state = WAIT;
									else
										next_state = SUB;
								end
								else
									next_state = C7;
							end
				state[9]:   begin	// C7
								if (current_note == notes[7]) begin
									if (current_note[7])
										next_state = WAIT;
									else
										next_state = SUB;
								end
								else
									next_state = ADD;
							end
				state[10]:  begin	// ADD
								next_state = WAIT;		
				   end
				state[11]:  begin	// SUB
								next_state = WAIT;		
				   			end
			endcase
		end
	end : next_state_decoder


	// output decoder		
	always_comb begin : output_decoder
		if (~rstn) begin
			interrupt = 1'b0;
		end
		else begin
			unique case (1'b1)
				state[ 0]: interrupt = 1'b0;
				state[ 1]: interrupt = 1'b0;
				state[ 2]: interrupt = 1'b0;
				state[ 3]: interrupt = 1'b0;
				state[ 4]: interrupt = 1'b0;
				state[ 5]: interrupt = 1'b0;
				state[ 6]: interrupt = 1'b0;
				state[ 7]: interrupt = 1'b0;
				state[ 8]: interrupt = 1'b0;
				state[ 9]: interrupt = 1'b0;
				state[10]: interrupt = 1'b1;
				state[11]: interrupt = 1'b1;
			endcase
		end
	end : output_decoder

    assign note_data = {1'b0, notes[7], 1'b0, notes[6], 1'b0, notes[5], 1'b0, notes[4],
                        1'b0, notes[3], 1'b0, notes[2], 1'b0, notes[1], 1'b0, notes[0]};
    
endmodule
