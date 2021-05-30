/////////////////////////////////////////////////////////
//
//	synchronizer.sv
//
//	This module will synchronize data sent from the
//	100 MHz Microblaze clock domain to the 18.432 MHz
//	I2S audio clock domain
//
//	Adapted from Portland State University ECE 540
//	lecture notes by Roy Kravitz and David Burnett
//
/////////////////////////////////////////////////////////

module synchronizer
(
	// a clock domain ports
	input wire aClk,
	input wire aResetn,
	input wire aWrite,
	input wire [23:0] aData,
	output logic aFull,
	output logic aReady,

	// b clock domain ports
	input wire bClk,
	input wire bResetn,
	input wire bRead,
	output logic [23:0] bData,
	output logic bFull
);

// state names
localparam EMPTY = 0, FULL = 1, TAKEN = 2;

// aClk domain registers
logic aSync1, aSync2;
logic arSync1, arSync2;
logic [23:0] aDataQ;

// bClk domain registers
logic bSync1, bSync2;
logic bReady;
logic [1:0] bState = EMPTY;

// aClk control logic
always @(posedge aClk or negedge aResetn)
	if (~aResetn) begin
		aFull <= 0;
		aSync1 <= 0;
		aSync2 <= 0;
		arSync1 <= 0;
		arSync2	<= 0;
		aReady <= 0;
	end 
	else begin
		// synchronizer
		{aSync2, aSync1} <= {aSync1, bFull};
		{arSync2, arSync1} <= {arSync1, bReady};

		// Full flag
		case (aFull)
			0: if (aWrite) begin
				 aFull <= 1;
			   end
			1: if (aSync2) begin
				 aFull <= 0;	// aka bFull after synchronization
			   end
		endcase
		if (aWrite) begin
			aReady <= 1'b0;
			arSync1 <= 1'b0;
			arSync2 <= 1'b0;
		end
		else if (arSync2) begin
			aReady <= 1'b1;
		end
		else begin
			aReady <= 1'b0;
		end
	end

	// aClk data register
	always @(posedge aClk) begin
		if (aWrite) aDataQ <= aData;
	end

	// bClk control logic
	always @(posedge bClk or negedge bResetn) begin
		if (~bResetn) begin
			bState <= 0;
			bSync1 <= 0;
			bSync2 <= 0;
			bReady <= 1'b1;
		end
		else begin
			// synchronizer
			{bSync2, bSync1} <= {bSync1, aFull};

			// Full flag
			case (bState)
				EMPTY: begin
						if (bSync2) begin
							bState <= FULL;
						end
				 		bReady <= 1'b0;
					end
				FULL: begin
						if (bRead) begin
							bState <= TAKEN;
						end
					 	bReady <= 1'b0;
				 	end
				TAKEN: begin
						if (!bSync2) begin
					 		bState <= EMPTY;
						end
				 		bReady <= 1'b1;
					end
				default: begin
					   bState <= EMPTY;
					   bReady <= 1'b0;
					 end
			endcase
		end
	end

	// combinational full flag
	assign bFull = (bState == FULL);

	// bClk data register
	always @(posedge bClk) begin
		if (bFull) begin
			bData <= aDataQ;
		end
		else begin
			bData <= bData;
		end
	end

endmodule