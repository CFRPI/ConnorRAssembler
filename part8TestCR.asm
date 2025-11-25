; this file tests the cases not covered by other files
; it shuffles around data using [bx] and [bx+]
; it should print 20 twice, memory[50] = 10, memory[51] = 20
MOV BX 50
MOV CX 10
MOV [BX] CX
ADD CX [BX]
MOV [BX+1] CX
MOV AX [BX+1]
PUT
MOV DX 20
CMP [BX] DX
JNE 17
PUT
halt