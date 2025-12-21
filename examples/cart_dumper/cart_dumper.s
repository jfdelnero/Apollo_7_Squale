*
* Squale cartridge dumper program
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

PIAKEYB     EQU $F044
PIACART     EQU $F048

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

	; ------------------------
	; Reset Address counter...
	; ------------------------
	CLR PIACART+1
	CLR PIACART
	LDA #$2C
	STA PIACART+1

	LDA #$3C
	STA PIAKEYB+3
	LDA #$34
	STA PIAKEYB+3

	LDA #100
	STA GDP_Y_LSB

	LDX #reading_print
	JSR print_string

	; Load
	LDU #PIACART
	LDX #cart_buff
loopcpy:
	LDA ,U
	STA ,X+
	CMPX #(cart_buff+$8000)
	BNE loopcpy


	; -----------------------
	; Checksum
	; -----------------------

	; Cart checsum
	CLRA
	CLRB
	STD cart_checksum

	LDX #cart_buff
loopcheck:
	LDA ,X+
	STA TMP_ADD+1

	PSHS A
	LDD cart_checksum
	ADDD TMP_ADD
	STD cart_checksum
	PULS A

	CMPX #(cart_buff+$8000)
	BNE loopcheck

	CLR GDP_Y_MSB
	LDA #80
	STA GDP_Y_LSB

	LDX #checkmsg_print
	JSR print_string

	LDA cart_checksum
	JSR print_hex

	LDA cart_checksum+1
	JSR print_hex

	; -----------------------
	; Send
	; -----------------------

	JSR wait_video_chip

	CLR GDP_Y_MSB
	LDA #60
	STA GDP_Y_LSB

	LDX #sendmsg_print
	JSR print_string

	LDX #cart_buff
loop_send:
	LDA ,X+

	JSR send_byte

	CMPX #(cart_buff+$8000)
	BNE loop_send

	; ------------------
	; End...
	; ------------------

	CLR GDP_Y_MSB
	LDA #32
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

	LDA #$11
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
print_char:
	JSR wait_video_chip

	STA GDP_CMD


	RTS

***************************************************************
print_hexquartet:

	PSHS A
	ANDA #$0F

	CMPA #$09
	BHI SUP9

	ADDA #$30 ; '0'

	JSR wait_video_chip
	STA GDP_CMD

	PULS A

	RTS

SUP9:
	SUBA #10
	ADDA #$41 ; 'A'

	JSR wait_video_chip
	STA GDP_CMD

	PULS A

	RTS

***************************************************************
print_hex:

	PSHS A

	LSRA
	LSRA
	LSRA
	LSRA

	JSR print_hexquartet

	PULS A

	JSR print_hexquartet

	RTS

***************************************************************
* Variables

strtmsg_print:
	.asciz "Squale Cartridge Dumper"

reading_print:
	.asciz "Reading ..."

sendmsg_print:
	.asciz "Sending ..."

checkmsg_print:
	.asciz "Checksum (hex): "

endmsg_print:
	.asciz "Done"

TMP_ADD:
	.word $0000

cart_adr:
	.word $0000

cart_checksum:
	.word $0000

cart_buff:

