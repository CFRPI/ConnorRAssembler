; initialize registers
; if everything works as expected we should get exactly 9 5's and a 7
MOV AX 5
MOV CX 5
MOV BX 2
MOV DX 6
; just puting some data there for testing purposes
MOV [75] BX
; put a halt at memory 100
MOV [100] AX
; If the jumps work, for each block we should jump over one PUT and only put once
CMP AX [75]
JA [17]
PUT
PUT
CMP AX CX
JE [22]
PUT
PUT
CMP BX 10
JB [28]
PUT
PUT
CMP BX 2
JAE [34]
PUT
PUT
CMP BX 1
JAE [40]
PUT
PUT
CMP CX 5
JBE [46]
PUT
PUT
CMP CX 6
JBE [52]
PUT
PUT
CMP CX 4
JNE [58]
PUT
PUT
CMP CX DX
JNE [63]
PUT
PUT
; test jmp
; if jmp works, we should jump to another put at memory 99 then halt at 100
MOV AX 7
MOV [99] AX
JMP 99
halt