; initialize registers
; if everything works as expected, we should get 9 5's
MOV AX 5
MOV CX 5
MOV BX 2
MOV DX 8
; loading in data
MOV [75] BX
; 100 = halt
; if any of the jumps happen, we will quit the loop and not print enough 5's
MOV [100] AX
CMP BX AX
JAE [100]
PUT
CMP AX [75]
JBE [100]
PUT
CMP AX AX
JA [100]
PUT
CMP AX DX
JA [100]
PUT
CMP CX CX
JB [100]
PUT
CMP CX 2
JB [100]
PUT
CMP AX 1
JE [100]
PUT
CMP AX 100
JE [100]
PUT
CMP DX DX
JNE [100]
PUT
halt