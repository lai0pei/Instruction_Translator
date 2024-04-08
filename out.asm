// push constant 20
@20        
D=A
@R0
A=M
M=D
@R0
M=M+1
// neg
@R0
A=M-1
D=-M
M=D
// not
@R0
A=M-1
D=!M
M=D
(END)
@END
0;JMP
