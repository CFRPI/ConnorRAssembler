; initial data
; our goal is to find the larger of two numbers
MOV AX -2
MOV BX 34
; if AX <= BX, AX = BX
CMP AX BX
JA 8
MOV AX BX
; skip to here if AX was already greater
PUT
halt