MOV AX [_numA]
ADD AX [_numB]
CMO AX 10
JA [_ifcase]
JBE [_done]
PUT
_ifcase MOV AX 2
put
_done halt



_numA 50
_numB 0