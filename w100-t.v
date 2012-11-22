// This bench is used only for test in ABC
// 2012-10-25 14:40

module w100 ( clock,
	G0, G1, G2, G3,
	G10  );
	input clock;
	input G0, G1, G2, G3;
	output G10;
	reg G7;
	wire n5, n6, n6_1, n8, n9, n3_1;
	assign n5 = G0 & G1;
	assign n6 = ~G1;
	assign n6_1 = n6 & G2;
	assign n8 = n5 | n6_1;
	assign n9 = ~n8;
	assign n3_1 = G3;
	assign G10 = G7 & n9;
	always @ (posedge clock) begin
		G7 <= n3_1;
	end
endmodule
