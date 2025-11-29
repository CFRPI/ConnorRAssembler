; A = location of where new nodes are kept in memory
; R = location of the root
; B = first outer if in insert
; C = second outer if in insert
; D = first inner if in insert
; E = second inner if in insert
; F = check for null root in insert
; G = used for calling functions with runtime parameters
; H = used for calling functions with runtime parameters
; I = variable used to pass numbers to insert element
; J = First if statement in print tree
;
; A node struct has the following structure
; Node {
; value
; left
; right
; }
; I will be using -1 as a null pointer
MOV DX 10
MOV [I] DX
FUN [60] 2 [-1] [I]
MOV BX [9]
MOV [R] BX
MOV DX 5
MOV [I] DX
FUN [60] 2 [R] [I]
MOV DX 2
MOV [I] DX
FUN [60] 2 [R] [I]
MOV DX 7
MOV [I] DX
FUN [60] 2 [R] [I]
FUN [180] 1 [R]
halt










; inserts an element into a tree and returns the address
; of the inserted element
MOV BX [59]
MOV BX [BX+2]
MOV AX [BX]
MOV BX [A]
; write the value
MOV [BX] AX
; initialize left and right as null
MOV DX -1
MOV [BX+1] DX
MOV [BX+2] DX
; CX now stores the address of our struct
MOV CX BX
; get the root
; BX will now contain the address of the address of the root
MOV BX [59]
MOV BX [BX+1]
; if the root is null, skip inserting
CMP BX -1
JNE [F]
MOV BX [A]
ADD BX 3
MOV [A] BX
MOV AX BX
SUB AX 3
RET
; Compare our structs value with root's value
; BX now contains the address of the root
F MOV BX [BX]
CMP AX [BX]
JAE [B]
; this.value < root.value
MOV DX -1
CMP DX [BX+1]
JE [D]
; store root.left in [G]
MOV AX [BX+1]
MOV [G] AX
; store this.value in [H]
MOV BX CX
MOV DX [BX]
MOV AX DX
MOV [H] DX
; insert(root.left, this.value)
FUN [60] 2 [G] [H]
; we did not use the struct we created so we decrement [A] to reuse this memory
MOV DX [A]
SUB DX 3
MOV [A] DX
JMP [B]
; root.left = this
D MOV [BX+1] CX
B CMP AX [BX]
JBE [C]
; this.value > root.value
MOV DX -1
CMP DX [BX+2]
JE [E]
; store root.right in [G]
MOV AX [BX+2]
MOV [G] AX
; store this.value in [H]
MOV BX CX
MOV DX [BX]
MOV [H] DX
; insert(root.right, this.value)
FUN [60] 2 [G] [H]
; we did not use the struct we created so we decrement [A] to reuse this memory
MOV DX [A]
SUB DX 3
MOV [A] DX
JMP [C]
; root.right = this
E MOV [BX+2] CX
; Update A to next available spot
C MOV BX [A]
ADD BX 3
MOV [A] BX
MOV AX [A]
SUB AX 3
RET








; Print tree
MOV BX [179]
MOV BX [BX+1]
; with this line, bx contains the address of the struct
MOV BX [BX]
; AX = struct.value
MOV AX [BX]
PUT
; start of children
MOV AX -100
PUT
MOV AX [BX+1]
CMP AX -1
JE J
; left child is not null, recursively print it
; G contains the address of the left child
MOV AX BX
ADD AX 1
MOV [G] AX
FUN [180] 1 [G]
; if left child is null, just print -1
JMP K
J MOV AX -1
PUT
K MOV AX [BX+2]
CMP AX -1
JE L
; right child is not null, recursively print it
MOV AX BX
ADD AX 2
MOV [G] AX
FUN [180] 1 [G]
; right child is null, just print -1
JMP M
L MOV AX -1
PUT
; end of children
M MOV AX -100
PUT
RET

R
G
H
I
A 300