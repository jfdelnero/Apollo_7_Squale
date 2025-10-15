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
ay_snd_wr:
	PSHS A

* Tone Channel A

*   LDA #0
*   STA AY_BASE
*   LDA #$08
*   STA AY_BASE+1

*   LDA #1
*   STA AY_BASE
*   LDA #$01
*   STA AY_BASE+1

* Tone Channel B

*   LDA #2
*   STA AY_BASE
*   LDA #$08
*   STA AY_BASE+1

*   LDA #3
*   STA AY_BASE
*   LDA #$01
*   STA AY_BASE+1

* Tone Channel C
	* Fine
	LDA #4
	STA AY_BASE
	LDA #$02
	STA AY_BASE+1

	* Coarse
	LDA #5
	STA AY_BASE
	LDA #$01
	STA AY_BASE+1

	* Noise gen period
	*LDA #6
	*STA AY_BASE
	*LDA #$5
	*STA AY_BASE+1

	* Mixer control
	LDA #7
	STA AY_BASE
	* Enable C Channel
	LDA #$3B
	STA AY_BASE+1

	* Ampl A
	LDA #10
	STA AY_BASE
	LDA #$1F
	STA AY_BASE+1

	* Ampl B
	*LDA #11
	*STA AY_BASE
	*LDA #$1F
	*STA AY_BASE+1

	* Ampl C
	LDA #12
	STA AY_BASE
	LDA #$1F
	STA AY_BASE+1

	LDA #13
	STA AY_BASE
	LDA #$89
	STA AY_BASE+1

	LDA #14
	STA AY_BASE
	LDA #$11
	STA AY_BASE+1

	LDA #15
	STA AY_BASE
	LDA #$09
	STA AY_BASE+1

	PULS A

	RTS

***************************************************************
* Variables
curcolor:
	.byte $6

y_pos:
	.word $1800

speed:
	.word INITIAL_SPEED

string_to_print:
	.asciz "Hello World !"
