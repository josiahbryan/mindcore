# parity2.spec -- specification of input/target pairs for 5-bit parity
# parity2.spec,v 1.1 2003/10/07 22:41:32 petervr Exp
#
# In this file, binary inputs have been encoded as -1.0/1.0 instead
# of as 0.0/1.0 parity.spec. This improves training performance
# dramatically. Note that the binary outputs are still encoded as
# 0.0/1.0; outputs of the neural network will always lie in the
# interval (0,1).
# number of pairs
32
# number of inputs
5
# number of outputs
1
# input & output
-1.0 -1.0 -1.0 -1.0 -1.0
0.0
-1.0 -1.0 -1.0 -1.0 1.0
1.0
-1.0 -1.0 -1.0 1.0 -1.0
1.0
-1.0 -1.0 -1.0 1.0 1.0
0.0
-1.0 -1.0 1.0 -1.0 -1.0
1.0
-1.0 -1.0 1.0 -1.0 1.0
0.0
-1.0 -1.0 1.0 1.0 -1.0
0.0
-1.0 -1.0 1.0 1.0 1.0
1.0
-1.0 1.0 -1.0 -1.0 -1.0
1.0
-1.0 1.0 -1.0 -1.0 1.0
0.0
-1.0 1.0 -1.0 1.0 -1.0
0.0
-1.0 1.0 -1.0 1.0 1.0
1.0
-1.0 1.0 1.0 -1.0 -1.0
0.0
-1.0 1.0 1.0 -1.0 1.0
1.0
-1.0 1.0 1.0 1.0 -1.0
1.0
-1.0 1.0 1.0 1.0 1.0
0.0
1.0 -1.0 -1.0 -1.0 -1.0
1.0
1.0 -1.0 -1.0 -1.0 1.0
0.0
1.0 -1.0 -1.0 1.0 -1.0
0.0
1.0 -1.0 -1.0 1.0 1.0
1.0
1.0 -1.0 1.0 -1.0 -1.0
0.0
1.0 -1.0 1.0 -1.0 1.0
1.0
1.0 -1.0 1.0 1.0 -1.0
1.0
1.0 -1.0 1.0 1.0 1.0
0.0
1.0 1.0 -1.0 -1.0 -1.0
0.0
1.0 1.0 -1.0 -1.0 1.0
1.0
1.0 1.0 -1.0 1.0 -1.0
1.0
1.0 1.0 -1.0 1.0 1.0
0.0
1.0 1.0 1.0 -1.0 -1.0
1.0
1.0 1.0 1.0 -1.0 1.0
0.0
1.0 1.0 1.0 1.0 -1.0
0.0
1.0 1.0 1.0 1.0 1.0
1.0
