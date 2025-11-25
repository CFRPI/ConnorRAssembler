; BX is the sum, CX is the counter
MOV BX 0
MOV [100] BX
MOV CX 0
; While CX < 10
CMP CX 10
JAE 21
FUN 40 3 [100] 30 50
ADD CX 1
JMP 6
; loop is over, output sum and halt
MOV AX [100]
PUT
halt
















MOV BX [39]
; min
MOV CX [BX+2]
; max
MOV DX [BX+3]
; reference to modify
mov BX [BX+1]
GET
CMP AX CX
JB 46
CMP AX DX
JA 46
; memory[100] += AX
ADD AX [BX]
MOV [BX] AX
RET
