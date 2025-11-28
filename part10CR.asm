; A = Array
; F = fill array
; B = end of loop in fill array
; C = start of loop in fill array
; G = start of loop in print array
; E = end of loop in print array
; H = start of outer loop in sort array
; I = end of outer loop in sort array
; J = start of inner loop in sort array
; K = end of inner loop in sort array
; O = inner loop if statement in sort array
; D = general variable 1
; L = general variable 2
; M = general variable 3
; N = general variable 4
; P = general variable 5
; Fill array
FUN [25] 1 [A]
; Print array
FUN [55] 1 [A]
; Sort array
FUN [85] 1 [A]
; Print array
FUN [55] 1 [A]
halt




; Fill array
MOV BX [24]
; Array address
MOV BX [BX+1]
; D is the address of the start of the array
MOV [D] BX
GET
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





; Print Array
MOV BX [54]
; BX is start of the array
MOV BX [BX+1]
; DX = array length
MOV DX [BX+-1]
; CX is a counter
MOV CX 0
E CMP CX DX
JAE G
; loop body
MOV AX [BX]
PUT
ADD CX 1
ADD BX 1
JMP E
G RET










; Sort Array, bubble sort
MOV BX [84]
MOV BX [BX+1]
MOV DX [BX+-1]
SUB DX 1
; P stores the start of the array
MOV [P] BX
; DX now equals the length of the array minus 1
; AX is our counter for both loops
MOV AX 0
H CMP AX DX
JAE I
; outer loop body
; D stores out outer loop iterator so we can use AX in the inner loop
; L stores the outer loop upper bound for the same reason
MOV [D] AX
MOV [L] DX
MOV AX 0
; M means whether numbers have swapped here, set it to 0
MOV [M] AX
; DX now holds the length of the array minus the outer loop iterator
MOV DX [L]
SUB DX [D]
J CMP AX DX
JAE K
; inner loop body
; BX is a pointer to array[AX]
MOV BX [P]
ADD BX AX
; N = array[AX+1]
MOV CX [BX+1]
MOV [N] CX
; CX = array[AX]
; if array[AX] <=  array[AX+1], skip
MOV CX [BX]
CMP CX [N]
JBE O
; swap values
; array[AX+1] = array[AX]
MOV [BX+1] CX
; CX = array[AX+1] (its old value)
MOV CX [N]
MOV [BX] CX
; done swapping values
; mark that they were swapped
MOV CX 1
MOV [M] CX
O ADD AX 1
JMP J
; restore values
; with this, we are back in the outer loop
K MOV DX [L]
MOV AX [D]
; if the swapped variable is false, exit the outer loop
MOV BX 0
CMP BX [M]
JE I
; increment counter and loop back
ADD AX 1
JMP H
I RET

; general variables
D
L
M
N
P

; array
A