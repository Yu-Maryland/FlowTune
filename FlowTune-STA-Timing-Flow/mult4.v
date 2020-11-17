// Benchmark "Multi4" written by ABC on Tue Nov 17 10:46:22 2020

module Multi4 ( 
    a0, a1, a2, a3, b0, b1, b2, b3,
    m0, m1, m2, m3, m4, m5, m6, m7  );
  input  a0, a1, a2, a3, b0, b1, b2, b3;
  output m0, m1, m2, m3, m4, m5, m6, m7;
  wire n18, n19, n20, n21, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32,
    n33, n35, n36, n37, n38, n39, n40, n41, n42, n43, n44, n45, n46, n47,
    n48, n49, n50, n51, n52, n53, n55, n56, n57, n58, n59, n60, n61, n62,
    n63, n64, n65, n66, n67, n68, n69, n70, n71, n72, n73, n75, n76, n77,
    n78, n79, n80, n81, n82, n83, n84, n85, n86, n87, n88, n89, n91, n92,
    n93, n94, n95, n96, n97, n98;
  assign m0 = a0 & b0;
  assign n18 = a1 & b0;
  assign n19 = a0 & b1;
  assign n20 = n18 & n19;
  assign n21 = ~n18 & ~n19;
  assign m1 = ~n20 & ~n21;
  assign n23 = a2 & b0;
  assign n24 = a1 & b1;
  assign n25 = n23 & n24;
  assign n26 = ~n23 & ~n24;
  assign n27 = ~n25 & ~n26;
  assign n28 = n20 & n27;
  assign n29 = ~n20 & ~n27;
  assign n30 = ~n28 & ~n29;
  assign n31 = a0 & b2;
  assign n32 = n30 & n31;
  assign n33 = ~n30 & ~n31;
  assign m2 = ~n32 & ~n33;
  assign n35 = ~n25 & ~n28;
  assign n36 = a3 & b0;
  assign n37 = a2 & b1;
  assign n38 = n36 & n37;
  assign n39 = ~n36 & ~n37;
  assign n40 = ~n38 & ~n39;
  assign n41 = ~n35 & n40;
  assign n42 = n35 & ~n40;
  assign n43 = ~n41 & ~n42;
  assign n44 = a1 & b2;
  assign n45 = n43 & n44;
  assign n46 = ~n43 & ~n44;
  assign n47 = ~n45 & ~n46;
  assign n48 = n32 & n47;
  assign n49 = ~n32 & ~n47;
  assign n50 = ~n48 & ~n49;
  assign n51 = a0 & b3;
  assign n52 = n50 & n51;
  assign n53 = ~n50 & ~n51;
  assign m3 = ~n52 & ~n53;
  assign n55 = ~n45 & ~n48;
  assign n56 = ~n38 & ~n41;
  assign n57 = a3 & b1;
  assign n58 = ~n56 & n57;
  assign n59 = n56 & ~n57;
  assign n60 = ~n58 & ~n59;
  assign n61 = a2 & b2;
  assign n62 = n60 & n61;
  assign n63 = ~n60 & ~n61;
  assign n64 = ~n62 & ~n63;
  assign n65 = ~n55 & n64;
  assign n66 = n55 & ~n64;
  assign n67 = ~n65 & ~n66;
  assign n68 = a1 & b3;
  assign n69 = n67 & n68;
  assign n70 = ~n67 & ~n68;
  assign n71 = ~n69 & ~n70;
  assign n72 = n52 & n71;
  assign n73 = ~n52 & ~n71;
  assign m4 = ~n72 & ~n73;
  assign n75 = ~n69 & ~n72;
  assign n76 = ~n62 & ~n65;
  assign n77 = a3 & b2;
  assign n78 = n58 & n77;
  assign n79 = ~n58 & ~n77;
  assign n80 = ~n78 & ~n79;
  assign n81 = ~n76 & n80;
  assign n82 = n76 & ~n80;
  assign n83 = ~n81 & ~n82;
  assign n84 = a2 & b3;
  assign n85 = n83 & n84;
  assign n86 = ~n83 & ~n84;
  assign n87 = ~n85 & ~n86;
  assign n88 = ~n75 & n87;
  assign n89 = n75 & ~n87;
  assign m5 = ~n88 & ~n89;
  assign n91 = ~n85 & ~n88;
  assign n92 = ~n78 & ~n81;
  assign n93 = a3 & b3;
  assign n94 = ~n92 & n93;
  assign n95 = n92 & ~n93;
  assign n96 = ~n94 & ~n95;
  assign n97 = ~n91 & n96;
  assign n98 = n91 & ~n96;
  assign m6 = ~n97 & ~n98;
  assign m7 = n94 | n97;
endmodule


