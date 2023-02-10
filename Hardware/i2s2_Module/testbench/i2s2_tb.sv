/////////////////////////////////////////////////////////
//
//	i2s2_tb.sv
//	Author: Mel Murphy
//
//	Testbench for I2S module
//
//	adapted from code by Kiran Sivakumar 2018
//
/////////////////////////////////////////////////////////

module i2s2_tb();

logic clk, rstn;
logic mclk, mrstn;
logic [23:0] pcm_in;
logic ready, sent;
logic sclk, lrclk, sdout;

i2s2_top DUT(.*);

initial begin

	clk = 0;
	mclk = 0;
	rstn = 1;
	mrstn = 1;
	#100;
	rstn = 0;
	mrstn = 0;
	#1000
	rstn = 1;
	mrstn = 1;

end

always begin
	#2 clk = ~clk;
end

always begin
	#11 mclk = ~mclk;
end

initial begin

	@(negedge rstn);
	@(posedge rstn);
	@(ready);
	pcm_in = 24'hAAAAAA;
	sent = 1'b1;
	@(posedge clk);
	sent = 1'b0;
	@(posedge ready);
	pcm_in = 24'hFFFFFF;
	sent = 1'b1;
	@(posedge clk);
	sent = 1'b0;
	@(posedge ready);
	repeat(500) @(posedge clk);
	pcm_in = 24'hFFFFFF;
	sent = 1'b1;
	@(posedge clk);
	sent = 1'b0;
	@(posedge ready);
	pcm_in = 24'hFFFFFF;
	sent = 1'b1;
	@(posedge clk);
	sent = 1'b0;
	@(posedge ready);
	pcm_in = 24'h696969;
	sent = 1'b1;
	@(posedge clk);
	sent = 1'b0;
	@(posedge ready);
	repeat(1000) @(posedge clk);
	pcm_in = 24'h555555;
	sent = 1'b1;
	@(posedge clk);
	sent = 1'b0;
	@(posedge ready);
	pcm_in = 24'h000000;
	sent = 1'b1;
	@(posedge clk);
	sent = 1'b0;
	repeat(100000) @(posedge clk);
	$stop;

end

endmodule