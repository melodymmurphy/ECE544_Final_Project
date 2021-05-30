module i2s2_top
(
	input wire clk,				// system clock
	input wire rstn,			// system clock domain reset
	input wire mclk,			// audio clock domain
	input wire mrstn,			// audio clock domain reset
	input wire [23:0] pcm_in,	// PCM sample sent by system

	output logic ready,	// tells system synchronizer is ready to receive a new value
	input wire sent,		// tells synchronizer we've written a value

	output wire sclk,			// serial clock
	output wire lrclk,			// word clock
	output wire sdout			// serial audio data out
);

	logic [23:0] pcm_sync;			// synchronized PCM output 
	logic synchReady;			// indicates that synchronizer is ready to receive data
	logic synchFull;			// indicates that the synchronizer is full
	wire readPCM;				// indicates that the I2S is requesting data from the synchronizer
	wire PCMready;				// indicates PCM data is within audio clock domain and ready to be read by I2S

	i2s2 i2s2_inst
	(
		.mclk(mclk),
		.rstn(mrstn),
		.pcm(pcm_sync),
		.sclk(sclk),
		.lrclk(lrclk),
		.i2sdata(sdout),
		.readPCM(readPCM),
		.PCMready(PCMready)
	);

	synchronizer synch_inst
	(
		.aClk(clk),
		.aResetn(rstn),
		.aWrite(sent),
		.aData(pcm_in),
		.aFull(synchFull),
		.aReady(synchReady),
		.bClk(mclk),
		.bResetn(mrstn),
		.bRead(readPCM),
		.bData(pcm_sync),
		.bFull(PCMready)
	);

	always_ff @(posedge clk, negedge rstn) begin

		if (~rstn) begin
			ready = 1'b0;
		end 

		else begin
			if ((!synchFull) && (synchReady)) begin
				ready = 1'b1;
			end
			else begin
				ready = 1'b0;
			end
		end

	end //ff

endmodule