//boostrapping
@256
D=A
@R0
M=D
@300
D=A
@R1
M=D
@400
D=A
@R2
M=D
@3000
D=A
@R3
M=D
@3010
D=A
@R4
M=D
//function Sys.init 0
(Sys.init)
//push constant 4000	
@4000
D=A
@R0
A=M
M=D
@R0
M=M+1
//pop pointer 0
@R0
A=M-1
D=M
@R3
M=D
@R0
M=M-1
//push constant 5000
@5000
D=A
@R0
A=M
M=D
@R0
M=M+1
//pop pointer 1
@R0
A=M-1
D=M
@R4
M=D
@R0
M=M-1
//call Sys.main 0
//generate return address
@Sys.init$ret.1
D=A
@R0
A=M
M=D
@R0
M=M+1
//saved local
@R1
D=M
@R0
A=M
M=D
@R0
M=M+1
//saved argument
@R2
D=M
@R0
A=M
M=D
@R0
M=M+1
//saved this
@R3
D=M
@R0
A=M
M=D
@R0
M=M+1
//saved that
@R4
D=M
@R0
A=M
M=D
@R0
M=M+1
//reposition argument
@5
D=A
@R0
D=M-D
@R2
M=D
//saved stack to local
@R0
D=M
@R1
M=D
//call the function
@Sys.main
0;JMP
//return address
(Sys.init$ret.1)
//pop temp 1
@5
D=A
@1
D=D+A
@R14
M=D
@R0
A=M-1
D=M
@R14
A=M
M=D
@R0
M=M-1
//label LOOP
(Sys.init$LOOP)
//goto LOOP
@Sys.init$LOOP
0;JMP
//function Sys.main 5
(Sys.main)
@0
D=A
@R1
A=M+D
M=0
@1
D=A
@R1
A=M+D
M=0
@2
D=A
@R1
A=M+D
M=0
@3
D=A
@R1
A=M+D
M=0
@4
D=A
@R1
A=M+D
M=0
//push constant 4001
@4001
D=A
@R0
A=M
M=D
@R0
M=M+1
//pop pointer 0
@R0
A=M-1
D=M
@R3
M=D
@R0
M=M-1
//push constant 5001
@5001
D=A
@R0
A=M
M=D
@R0
M=M+1
//pop pointer 1
@R0
A=M-1
D=M
@R4
M=D
@R0
M=M-1
//push constant 200
@200
D=A
@R0
A=M
M=D
@R0
M=M+1
//pop local 1
@R0
A=M-1
D=M
@R0
M=M-1
@Sys.main$local.1
M=D
//push constant 40
@40
D=A
@R0
A=M
M=D
@R0
M=M+1
//pop local 2
@R0
A=M-1
D=M
@R0
M=M-1
@Sys.main$local.2
M=D
//push constant 6
@6
D=A
@R0
A=M
M=D
@R0
M=M+1
//pop local 3
@R0
A=M-1
D=M
@R0
M=M-1
@Sys.main$local.3
M=D
//push constant 123
@123
D=A
@R0
A=M
M=D
@R0
M=M+1
//call Sys.add12 1
//generate return address
@Sys.main$ret.2
D=A
@R0
A=M
M=D
@R0
M=M+1
//saved local
@R1
D=M
@R0
A=M
M=D
@R0
M=M+1
//saved argument
@R2
D=M
@R0
A=M
M=D
@R0
M=M+1
//saved this
@R3
D=M
@R0
A=M
M=D
@R0
M=M+1
//saved that
@R4
D=M
@R0
A=M
M=D
@R0
M=M+1
//reposition argument
@6
D=A
@R0
D=M-D
@R2
M=D
//saved stack to local
@R0
D=M
@R1
M=D
//call the function
@Sys.add12
0;JMP
//return address
(Sys.main$ret.2)
//pop temp 0
@5
D=A
@0
D=D+A
@R14
M=D
@R0
A=M-1
D=M
@R14
A=M
M=D
@R0
M=M-1
//push local 0
@Sys.main$local
D=M
@R0
A=M
M=D
@R0
M=M+1
//push local 1
@Sys.main$local.1
D=M
@R0
A=M
M=D
@R0
M=M+1
//push local 2
@Sys.main$local.2
D=M
@R0
A=M
M=D
@R0
M=M+1
//push local 3
@Sys.main$local.3
D=M
@R0
A=M
M=D
@R0
M=M+1
//push local 4
@Sys.main$local.4
D=M
@R0
A=M
M=D
@R0
M=M+1
//add
@R0
A=M-1
D=M
A=A-1
D=D+M
M=D
@R0
M=M-1
//add
@R0
A=M-1
D=M
A=A-1
D=D+M
M=D
@R0
M=M-1
//add
@R0
A=M-1
D=M
A=A-1
D=D+M
M=D
@R0
M=M-1
//add
@R0
A=M-1
D=M
A=A-1
D=D+M
M=D
@R0
M=M-1
//return
//saved frame pointer to temp variable
@R1
D=M
@Sys.main$frame_pointer
M=D
//saved return address to temp variable
@5
D=A
@Sys.main$frame_pointer
A=M-D
D=M
@Sys.main$retAddr
M=D
//save return value
@R0
A=M-1
D=M
@R2
A=M
M=D
//restore stack pointer
@R2
D=M+1
@R0
M=D
//restore that pointer
@Sys.main$frame_pointer
A=M-1
D=M
@R4
M=D
//restore this pointer
@2
D=A
@Sys.main$frame_pointer
A=M-D
D=M
@R3
M=D
//restore arg pointer
@3
D=A
@Sys.main$frame_pointer
A=M-D
D=M
@R2
M=D
//restore lcl pointer
@4
D=A
@Sys.main$frame_pointer
A=M-D
D=M
@R1
M=D
//return
@Sys.main$retAddr
A=M
0;JMP
//function Sys.add12 0
(Sys.add12)
//push constant 4002
@4002
D=A
@R0
A=M
M=D
@R0
M=M+1
//pop pointer 0
@R0
A=M-1
D=M
@R3
M=D
@R0
M=M-1
//push constant 5002
@5002
D=A
@R0
A=M
M=D
@R0
M=M+1
//pop pointer 1
@R0
A=M-1
D=M
@R4
M=D
@R0
M=M-1
//push argument 0
@R2
D=M
@0
A=D+A
D=M
@R0
A=M
M=D
@R0
M=M+1
//push constant 12
@12
D=A
@R0
A=M
M=D
@R0
M=M+1
//add
@R0
A=M-1
D=M
A=A-1
D=D+M
M=D
@R0
M=M-1
//return
//saved frame pointer to temp variable
@R1
D=M
@Sys.add12$frame_pointer
M=D
//saved return address to temp variable
@5
D=A
@Sys.add12$frame_pointer
A=M-D
D=M
@Sys.add12$retAddr
M=D
//save return value
@R0
A=M-1
D=M
@R2
A=M
M=D
//restore stack pointer
@R2
D=M+1
@R0
M=D
//restore that pointer
@Sys.add12$frame_pointer
A=M-1
D=M
@R4
M=D
//restore this pointer
@2
D=A
@Sys.add12$frame_pointer
A=M-D
D=M
@R3
M=D
//restore arg pointer
@3
D=A
@Sys.add12$frame_pointer
A=M-D
D=M
@R2
M=D
//restore lcl pointer
@4
D=A
@Sys.add12$frame_pointer
A=M-D
D=M
@R1
M=D
//return
@Sys.add12$retAddr
A=M
0;JMP
(END)
@END
0;JMP
