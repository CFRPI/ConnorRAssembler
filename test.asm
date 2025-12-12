FUN [_fun] 1 5
halt

_fun:
ADDRV BX [_fun]
MOV BX [BX+-1]
MOV BX [BX+1]
MOV DX BX
ADDRV BX [_array]
MOV [BX+-1] 5
MOV CX 0
_funLoopStart:
CMP CX DX
JAE [_funDone]
GET
MOV [BX] AX
ADDI BX 1
ADDI CX 1
JMP [_funLoopStart]
_funDone:
RET


_array: