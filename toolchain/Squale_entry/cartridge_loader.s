# Squale Cardridge Loader
# (c) HxC2001 

 .area .text
 .globl __start

PIAROM EQU $F048
END_ADR EQU $C100
BASE_ADR EQU $0000

 ORG $C100

__start:

STARTLOADER
 ldu #PIAROM
 ldx #BASE_ADR
LOOPCPY 
 lda ,U
 sta ,X+
 cmpx #END_ADR
 bne LOOPCPY

 jmp >$0000

