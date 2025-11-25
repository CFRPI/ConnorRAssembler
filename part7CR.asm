; BX is the sum, CX is the counter
MOV BX 0
MOV CX 0
; While CX < 10
CMP CX 10
JAE 13
GET
ADD BX AX
ADD CX 1
JMP 4
; loop is over, output sum and halt
MOV AX BX
PUT
halt