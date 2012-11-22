// Benchmark "exp/w100" written by ABC on Thu Oct 18 20:25:58 2012

module \exp/w100  ( clock, 
    G0, G1, G2, G3,
    G10  );
  input  clock;
  input  G0, G1, G2, G3;
  output G10;
  reg G7;
  wire n8, n9, n10_1, n11, n12, n10;
  assign n8 = ~G1;
  assign n9 = ~n12;
  assign n10_1 = G0 & G1;
  assign n11 = G2 & n8;
  assign n12 = n10_1 ^ n11;
  assign G10 = ~G7 & ~n9;
  assign n10 = G3;
  always @ (posedge clock) begin
    G7 <= n10;
  end
endmodule


