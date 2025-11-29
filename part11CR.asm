; A = location of where new nodes are kept in memory
;
; A node struct has the following structure
; Node {
; value
; left
; right
; }
; I will be using -1 as a null pointer
; I always pass by reference since both functions are recursive and need runtime arguments
;
; DX is the value to insert into the tree
; insert 10, creates root node
MOV DX 10
MOV [I] DX
FUN [60] 2 [-1] [I]
MOV BX [9]
MOV [R] BX
;insert 15
MOV DX 15
MOV [I] DX
FUN [60] 2 [R] [I]
; insert 2
MOV DX 2
MOV [I] DX
FUN [60] 2 [R] [I]
; insert 7
MOV DX 7
MOV [I] DX
FUN [60] 2 [R] [I]
; DX is now the value to search for
MOV DX 15
MOV [I] DX
FUN [180] 2 [I] [R]
MOV AX [53]
; Prints 1 if the value is found in the tree, otherwise 0
PUT
halt


; === Insert Element===
; inserts an element into a tree and returns the address
; of the inserted element
; Arguments
; - 1. the root of the binary tree to insert into, -1 means this will be the root node
; - 2. the address of the value to insert
; Return - the address of the node that was just created
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
; root.left was null, set root.left = this
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
; root.right was null so set root.right = this
E MOV [BX+2] CX
; Update A to next available spot
C MOV BX [A]
ADD BX 3
MOV [A] BX
MOV AX [A]
SUB AX 3
RET








; ===Search Tree===
; Searches for a value in a binary search tree
; Args:
; 1. address of value to search for
; 2. address of root of tree
; Return - 1 if the value was found, 0 otherwise
MOV BX [179]
MOV BX [BX+1]
; DX = value to search for
MOV DX [BX]
MOV AX DX
MOV BX [179]
; BX = address of root
MOV BX [BX+2]
MOV BX [BX]
MOV AX BX
; Checks if value to search for = value of root
CMP DX [BX]
JNE K
; value was found, return true
MOV AX 1
RET
K CMP DX [BX]
JA L
; value < root.value
MOV CX -1
CMP CX [BX+1]
JE M
; left node is not null, search left node recursively
MOV [G] DX
MOV CX [BX+1]
MOV [H] CX
FUN [180] 2 [G] [H]
; return recursive call, !HARDCODED ADDRESS
MOV AX [218]
RET
; there is no left node, value not found in tree
M MOV AX 0
RET
; L means value > root.value
L MOV CX -1
CMP CX [BX+2]
JE N
; right node is not null, search right node recursively
MOV [G] DX
MOV CX [BX+2]
MOV [H] CX
FUN [180] 2 [G] [H]
; return recursive call, !HARDCODED ADDRESS
MOV AX [242]
RET
; N means there is no right node so value is not found
N MOV AX 0
RET

; Location of the root
R
; Used to call functions with runtime arguments
G
; Used to call functions with runtime arguments
H
; Used to call functions with arguments in the main program
I
; Address that structs will be created at, updated each time a new struct is made
A 300