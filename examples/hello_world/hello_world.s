*
* The bouncing "Hello world !" program for Squale
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

AY_BASE     EQU $F060

INITIAL_SPEED       EQU 1664

__main:

****************************************************************************************
* Main loop

main_loop:

	* Previous GPD process done ?
	JSR wait_video_chip

	* VBlank wait
	JSR wait_vblank

	* Erase previous string - if position changed
	* ( write again the string with the background color)
	LDA y_pos
	CMPA GDP_Y_LSB
	BEQ skip_erase

	LDA #$E
	STA GDP_COLOR

	LDX #string_to_print
	JSR print_string

skip_erase:

	* Set the new Y position
	CLR GDP_Y_MSB
	LDD y_pos
	STA GDP_Y_LSB

	* Move the string
	LDD  speed
	ADDD y_pos
	STD  y_pos

	* Reduce the speed/energy for the next frame
	LDD speed
	SUBD #1
	STD speed

	* Floor touched ?
	LDD  y_pos
	CMPD #$1800
	BHI  noinv

	************************************
inv:
	* Floor reached :
	* - Change the color
	* - negate and reduce the speed / energy
	* - if the energy is near to 0 - reset it to its max/initial value.

	JSR ay_snd_wr

	***
	* Change color
	LDA curcolor         ; Color
	INCA
	ANDA #$F
	CMPA #$E
	BNE dontskipcol
	* Skip he background color
	INCA
	ANDA #$F
dontskipcol:
	STA curcolor         ; Color
	*
	***

	* speed = -speed
	LDD  #0
	SUBD speed
	STD speed

	* energy loss
	* speed -= 128
	LDD speed
	SUBD #128
	STD speed

	* speed < 128 ?
	* no more speed/energy ?
	CMPD #128
	BHI noreload
	* reinit speed value
	LDD #INITIAL_SPEED

noreload:
	STD  speed

	* Fix the y position
	LDD  #$1800
noinv:
************************************
	STD  y_pos

	* Previous GPD process done ?
	JSR wait_video_chip

	* Set the color
	LDA curcolor         ; Color
	STA GDP_COLOR

	* Print the string
	LDX #string_to_print
	JSR print_string

	* Apply the gravity effect
	LDD speed
	SUBD #32
	STD speed

	JMP main_loop

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

***************************************************************
*clear_screen:
*   JSR wait_video_chip
*   LDA #$4
*   STA GDP_CMD
*   RTS

***************************************************************
print_string:

*   JSR wait_video_chip

	CLR GDP_X_MSB
	LDA #$30
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
* sound event
ay_snd_wr:
	PSHS A

*
* Warning : AY-3-8910 Datasheet RX registers indexes are in octal !
*

	LDY #AY_BASE
	LDX #soundprg

	LDA #0
loopprg:
	LDB ,X+
	STD ,Y
	INCA
	CMPA #$14
	BLO loopprg

	PULS A

	RTS

***************************************************************
* Variables

soundprg:
	* Tone A Fine (R0) (TP7-TP0), Tone A Coarse (R1) (TP11-TP8)
	.byte $2, $1
	* Tone B Fine (R2) (TP7-TP0), Tone B Coarse (R3) (TP11-TP8)
	.byte $4, $1
	* Tone C Fine (R4) (TP7-TP0), Tone C Coarse (R5) (TP11-TP8)
	.byte $8, $1

	* Noise gen period (R6)
	.byte $1F

	* Mixer control (R7)
	*B7: /Input B enable
	*B6: /Input A enable
	*B5: /Noise C enable
	*B4: /Noise B enable
	*B3: /Noise A enable
	*B2: /Tone C enable
	*B1: /Tone B enable
	*B0: /Tone A enable
	.byte $38

	* Ampl A (R10)
	* B3-B0: Fixed amplitude
	* B4: Envelope mode enable
	.byte $10

	* Ampl B (R11)
	* B3-B0: Fixed amplitude
	* B4: Envelope mode enable
	.byte $10

	* Ampl C (R12)
	* B3-B0: Fixed amplitude
	* B4: Envelope mode enable
	.byte $10

	* Envelope Fine Tune (R13), Envelope Coarse Tune (R14)
	.byte $89,$30

	* Envelope Shape/Cycle control (R15)
	* B3: Continue
	* B2: Attack
	* B1: Alternate
	* B0: Hold
	*
	* _____/|______
	*
	.byte $00

curcolor:
	.byte $6

y_pos:
	.word $1800

speed:
	.word INITIAL_SPEED

string_to_print:
	.asciz "Hello World !"
