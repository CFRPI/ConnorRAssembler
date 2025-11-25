; A = Array
; F = fill array
; B = end of loop in fill array
; C = start of loop in fill array
; D = general variable 1
FUN [7] 1 [A]
halt

; Fill array
MOV BX [6]
; Array address
MOV BX [BX+1]
; D is the address of the start of the array
MOV [D] BX
GET
MOV BX [D]
MOV [BX+-1] AX
MOV DX AX
; CX is our index
MOV CX 0
C CMP CX DX
JAE B
; loop body
GET
MOV BX [D]
ADD BX CX
MOV [BX] AX
ADD CX 1
JMP C
B RET


D

A