*
* The "Hello world !" program for Squale
*
* (c) 2025 HxC2001 / Jean-François DEL NERO
*

 .area .text
 .globl __main

GDP_CMD     EQU $F000
GDP_CSIZE   EQU $F003
GDP_DX      EQU $F005
GDP_DY      EQU $F007
GDP_X_MSB   EQU $F008
GDP_X_LSB   EQU $F009
GDP_Y_MSB   EQU $F00A
GDP_Y_LSB   EQU $F00B
GDP_COLOR   EQU $F010

__main:

main_loop:
	JSR wait_video_chip

	LDA curcolor         ; Color
	INCA
	ANDA #$F
	STA curcolor         ; Color
	STA GDP_COLOR

	JSR clear_screen

	JSR wait_video_chip

	LDA curcolor         ; Color
	EORA #$8
	STA GDP_COLOR

	LDX #string_to_print
	JSR print_string

	LDA #50
delay_loop:
	JSR wait_vblank
	DECA
	BNE delay_loop

	JMP main_loop

deadloop:
	JMP deadloop

***************************************************************
wait_video_chip:
	PSHS A

wait_video_chip_w:
	LDA GDP_CMD
	ANDA #4
	BEQ wait_video_chip_w

	PULS A

	RTS

***************************************************************
wait_vblank:
	PSHS A

wait_vblank_vsync_set:
	LDA GDP_CMD
	ANDA #$02
	BEQ wait_vblank_vsync_set

wait_vblank_vsync_clr:
	LDA GDP_CMD
	ANDA #$02
	BNE wait_vblank_vsync_clr

	PULS A

	RTS

***************************************************************
clear_screen:
	JSR wait_video_chip
	LDA #$4
	STA GDP_CMD
	RTS

***************************************************************
print_string:

	JSR wait_video_chip

	CLR GDP_X_MSB
	LDA #$10
	STA GDP_X_LSB

	CLR GDP_Y_MSB
	LDA #$80
	STA GDP_Y_LSB

	LDA #$33
	STA GDP_CSIZE

print_loop:
	LDA ,X+
	BEQ print_end
	STA GDP_CMD

	JSR wait_video_chip

	BRA print_loop

print_end:
	RTS

***************************************************************
curcolor:
	.byte $6

string_to_print:
	.asciz "Hello World !"

