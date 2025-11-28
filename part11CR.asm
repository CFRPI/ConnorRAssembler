; A = location of where new nodes are kept in memory
; R = location of the root
; B = first outer if in insert
; C = second outer if in insert
; D = first inner if in insert
; E = second inner if in insert
; F = check for null root in insert
; G = used for calling functions with runtime parameters
; H = used for calling functions with runtime parameters
;
; A node struct has the following structure
; Node {
; value
; left
; right
; }
; I will be using -1 as a null pointer
FUN [40] 2 [-1] 10
MOV BX [5]
MOV [R] BX
FUN [40] 2 [R] 5
FUN [40] 2 [R] 2
halt

















; inserts an element into a tree and returns the address
; of the inserted element
MOV BX [39]
MOV AX [BX+2]
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
MOV BX [39]
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
FUN [40] 2 [G] [H]
; we did not use the struct we created so we decrement [A] to reuse this memory
MOV DX [A]
SUB DX 3
MOV [A] DX
JMP [B]
; root.left = this
D MOV [BX+1] CX
B CMP DX [BX+2]
JBE [C]
; this.value > root.value
MOV DX -1
CMP DX [BX+1]
JE [E]
; store root.right in [G]
MOV AX [BX+2]
MOV [G] AX
; store this.value in [H]
MOV BX CX
MOV DX [BX]
MOV [H] DX
; insert(root.right, this.value)
FUN [40] 2 [G] [H]
; we did not use the struct we created so we decrement [A] to reuse this memory
MOV DX [A]
SUB DX 3
MOV [A] DX
JMP [C]
; root.left = this
E MOV [BX+2] CX
; Update A to next available spot
C MOV BX [A]
ADD BX 3
MOV [A] BX
MOV AX [A]
SUB AX 3
RET

R
G
H
A 200
; 141
; 143
; 145
; 147