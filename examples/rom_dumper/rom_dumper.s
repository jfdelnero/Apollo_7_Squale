*
* Squale ROM dumper program
*
* (c) 2025 HxC2001 / Jean-François DEL NERO
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

ACIA        EQU $F050

AY_BASE     EQU $F060

__main:

****************************************************************************************
* Main loop

main_loop:

	CLR GDP_Y_MSB
	LDA #128
	STA GDP_Y_LSB

	* Previous GPD process done ?
	JSR wait_video_chip

	* VBlank wait
	JSR wait_vblank

	LDA #$10       ; sys reg : 1200 baud + black
	STA GDP_COLOR

	LDX #strtmsg_print
	JSR print_string

	; 0001 1000
	LDA #$1B       ; ACIA : Master reset
	STA ACIA

	LDA #$19       ; ACIA : 8 bits + even parity + 19200/16=1200 Baud
	STA ACIA

	LDA #250
	JSR delay_vlb

	; ------------------
	; First page...
	; ------------------

	LDA #(64)
	STA GDP_Y_LSB

	LDX #page1_print
	JSR print_string

	; ROM base address
	LDX #$F100

loop_dump_p1:
	LDA ,X+
	JSR send_byte

	CMPX #$0000
	BNE loop_dump_p1

	; inter page pause

	LDA #100
	JSR delay_vlb

	; ------------------
	; Second page...
	; ------------------

	LDA #(64 - 16)
	STA GDP_Y_LSB

	LDX #page2_print
	JSR print_string

	LDA #$90       ; sys reg : 1200 baud + black + second bank
	STA GDP_COLOR

	; ROM base address
	LDX #$F100

loop_dump_p2:
	LDA ,X+
	JSR send_byte

	CMPX #$0000
	BNE loop_dump_p2

	; ------------------
	; End...
	; ------------------

	CLR GDP_Y_MSB
	LDD #32
	STA GDP_Y_LSB

	LDX #endmsg_print
	JSR print_string

end_loop:
	JMP end_loop

* Main loop end
****************************************************************************************

***************************************************************
send_byte:
	PSHS A

loop_wait_send_byte:
	LDA ACIA
	ANDA #$02
	BEQ loop_wait_send_byte

	PULS A

	STA ACIA+1

	RTS

***************************************************************
delay_vlb:

	JSR wait_vblank

	DECA
	BNE delay_vlb

	RTS

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
*clear_screen:
*   JSR wait_video_chip
*   LDA #$4
*   STA GDP_CMD
*   RTS

***************************************************************
print_string:

	JSR wait_video_chip

	CLR GDP_X_MSB
	LDA #$00
	STA GDP_X_LSB

*   CLR GDP_Y_MSB
*   LDA #$80
*   STA GDP_Y_LSB

	LDA #$21
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
* Variables

strtmsg_print:
	.asciz "Squale ROM Dumper"

page1_print:
	.asciz "Page 1 ..."

page2_print:
	.asciz "Page 2 ..."

endmsg_print:
	.asciz "Done"

