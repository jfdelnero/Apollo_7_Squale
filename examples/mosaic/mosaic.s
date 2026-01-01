*
* Mosaic test program for Squale
*
* (c) 2026 HxC2001 / Jean-François DEL NERO
*

 .area .text
 .globl __main

*
* EF9365 GDP Registers definitions
*

GDP_CMD     EQU $F000
GDP_CTRL1   EQU $F001
GDP_CSIZE   EQU $F003
GDP_DX      EQU $F005
GDP_DY      EQU $F007
GDP_X_MSB   EQU $F008
GDP_X_LSB   EQU $F009
GDP_Y_MSB   EQU $F00A
GDP_Y_LSB   EQU $F00B
GDP_COLOR   EQU $F010

__main:

****************************************************************************************
* Main loop

main_loop:

	* Previous GPD process done ?
	JSR wait_video_chip

	CLR GDP_Y_MSB
	CLR GDP_Y_LSB

	CLR GDP_X_MSB
	CLR GDP_X_LSB

	CLR GDP_DY
	LDA #$FF    ; Fill the line from bottom to top
	STA GDP_DY

	CLR GDP_DX

loop_fill_screen:

	LDA curcolor         ; Color
	STA GDP_COLOR

	* Reset Y position (back to the bottom line)
	CLR GDP_Y_LSB

	LDA x_pos
	STA GDP_X_LSB
	INCA
	STA x_pos

	* Send the line command to the GDP
	LDA #$11
	STA GDP_CMD

	* Wait the end of the line drawing
	JSR wait_video_chip

	LDA x_pos            ; 16 lines Color bar done ?
	ANDA #$F
	BNE loop_fill_screen

	* Next bar

	* Color Change
	LDA curcolor
	INCA
	ANDA #$F
	STA curcolor

	LDA x_pos            ; Full screen done ?
	CMPA #$00
	BNE loop_fill_screen

	LDA #$50

	* wait some seconds
pause_loop:
	JSR wait_vblank
	DECA
	BNE pause_loop

	* Shift the color bars and restart

	LDA curcolor
	INCA
	ANDA #$F
	STA curcolor

	CLR GDP_Y_MSB
	CLR GDP_Y_LSB

	CLR GDP_X_MSB
	CLR GDP_X_LSB

	JMP loop_fill_screen

* Main loop end
****************************************************************************************

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

curcolor:
	.byte $00

x_pos:
	.byte $00

