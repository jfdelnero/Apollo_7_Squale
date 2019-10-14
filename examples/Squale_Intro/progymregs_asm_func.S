; Apollo Squale Intro
; (c) 2016 HxC2001 / Jean-François DEL NERO

 .area .text
 .globl _progymregs_asm_func

_progymregs_asm_func:
	pshs	d,y,u
	leau	,s

	ldy #$F060

	LDA #$00
	LDB ,X+
	STD ,Y

	LDA #$01
	LDB ,X+
	STD ,Y
	 
	LDA #$02
	LDB ,X+
	STD ,Y
	 
	LDA #$03
	LDB ,X+
	STD ,Y

	LDA #$04
	LDB ,X+
	STD ,Y
	 
	LDA #$05
	LDB ,X+
	STD ,Y

	LDA #$06
	LDB ,X+
	STD ,Y
	 
	LDA #$07
	LDB ,X+
	STD ,Y

	LDA #$08
	LDB ,X+
	STD ,Y

	LDA #$09
	LDB ,X+
	STD ,Y

	LDA #$0A
	LDB ,X+
	STD ,Y

	LDA #$0B
	LDB ,X+
	STD ,Y

	LDA #$0C
	LDB ,X+
	STD ,Y

	LDA #$0D
	LDB ,X+
	STD ,Y

	puls   d,y,u,pc

