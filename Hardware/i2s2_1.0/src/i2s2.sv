/////////////////////////////////////////////////////////
//
//	I2S2.sv
//	Author: Mel Murphy
//
//	This module will take a PCM input and convert it
//	to an I2S sigal for output through the PMOD I2S2
//	module.
//
//	adapted from code by Kiran Sivakumar 2018, and
//	modifications made to that code by David Craft, 2021
//
/////////////////////////////////////////////////////////

module i2s2
(
		input wire mclk,			// 18.432 MHz or 384 * sample rate of 48 kHz
		input wire rstn,			// reset signal
		input logic [23:0] pcm,		// incoming PCM sample
		output logic readPCM,			// signal to read value from synchronizer
		input wire PCMready,		// tells the module when there is data available
		output logic lrclk,			// 48 kHz, same as sample rate
		output logic sclk,			// 2.304 MHz or 48 * sample rate
		output logic i2sdata		// output I2S signal
);

logic [23:0] sample;		// sample that is currently being sent over serial data out
logic [7:0] lr_counter;		// counter for word clock
logic [1:0] s_counter;		// counter for serial clock
logic getNewSample;			// store whether we want to send a signal to read a new sample or not

always_ff @(posedge mclk, negedge rstn) begin

	if (~rstn) begin		// on reset, initialize all values to zero
		s_counter <= '0;
		lr_counter <= '0;
		sample <= '0;
		lrclk <= 0;
		sclk <= 0;
		i2sdata <= 0;
		getNewSample <= 1'b0;
	end

	else begin
		if (s_counter == 3) begin
            sclk <= ~sclk;				// toggle serial clock every 4 clock ticks

            if (sclk) begin					// on negative edge of serial clock, output next bit of sample 
            	i2sdata <= sample[23];
             		sample <= sample << 1;			// shift the next bit into position
            end
            else begin
            	i2sdata <= i2sdata;
            end
		end
		else begin
			sclk <= sclk;
			sample <= sample;
		end

        if (lr_counter == 191) begin			// every 24 bits sent (every 24 8-tick intervals), toggle the word clock
            lrclk <= ~lrclk;					// toggle word clock
            sample <= pcm;						// read pending sample into sample register
            getNewSample <= 1'b1;				// signal that 
            lr_counter <= 0;					// reset word clock		
        end
		else begin
			lrclk <= lrclk;
			lr_counter <= lr_counter + 1;
			if (getNewSample && PCMready) begin
				readPCM <= 1'b1;
				getNewSample <= 1'b0;
			end
			else begin
				readPCM <= 1'b0;
				getNewSample <= getNewSample;
			end
		end

		s_counter <= s_counter + 1;		// increment serial clock

	end



end // ff


endmodule