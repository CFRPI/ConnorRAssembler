; BX is the sum, CX is the counter
MOV BX 0
MOV CX 0
; While CX < 10
CMP CX 10
JAE 21
FUN 40 2 30 50
MOV AX [13]
ADD BX AX
ADD CX 1
JMP 4
; loop is over, output sum and halt
MOV AX BX
PUT
halt
















MOV BX [39]
; min
MOV CX [BX+1]
; max
MOV DX [BX+2]
GET
CMP AX CX
JB 46
CMP AX DX
JA 46
; if CX <= AX <= DX, return AX
RET
