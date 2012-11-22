// This test is quite simple
// 2012-10-29 14:08

module w100 (
	G0, G1,
	G2  );
	input G0, G1;
	output G2;
	assign G2 = G0 & G1;
endmodule
