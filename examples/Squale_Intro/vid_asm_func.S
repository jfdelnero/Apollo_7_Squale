; Apollo Squale Intro
; (c) 2016 HxC2001 / Jean-François DEL NERO

 .area .text
 .globl _vid_asm_func

_vid_asm_func:
	pshs	y,u
	leau	,s

	LDY #$F000
	STB ,Y

	puls	y,u,pc
