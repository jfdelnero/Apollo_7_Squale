;;;
;;; Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
;;;
;;; This file is part of GCC.
;;;
;;; GCC is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 3, or (at your option)
;;; any later version.
;;;
;;; GCC is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.

;;; You should have received a copy of the GNU General Public License
;;; along with GCC; see the file COPYING3.  If not see
;;; <http://www.gnu.org/licenses/>.

#undef TARGET_COCO
#define TARGET_PROTO 1

	org 0
	/* Declare external for main() */
	.globl	_main

/* The startup is heavily dependent on the type of machine and
OS environment that is available at the start point.
For the most part, the general idea is the same across machines,
but the implementation is vastly different.  This is managed via
conditional compiles throughout the startup code for each of the
supported machines. */

#if defined(TARGET_COCO) /* CoCo memory map */

#define COCO_RAMROM_MODE 0xFFDE
#define COCO_ALLRAM_MODE 0xFFDF
#define COCO_PAGE1 0xFFD5

/* SAM M1 and M0 adjusts the memory size */

#define BASIC_WARMSTART_FLAG 0x0071
#define BASIC_START 0xA027

#define __STACK_TOP 0x6800

#elif defined(TARGET_PROTO) /* Prototype hardware. Customisation of this section is expected! */

#define __STACK_TOP 0xE600

#else /* Simulator (default) memory map */

#define SIM_EXIT_REG 0xFF01

#define __STACK_TOP 0xFE00

#endif

	.area	.data
	.area	.ctors
	.area	.dtors
	.area	.bss

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;
	;;; __exit : Exit point from the program
	;;; For simulation, this writes to a special I/O register that
	;;; the simulator interprets as end-of-program.
	;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.area	.text
 	.globl	__exit
__exit:
#if defined(TARGET_COCO)
	;; Go back to ROM/RAM mode
	sta	COCO_RAMROM_MODE
	clr	BASIC_WARMSTART_FLAG
	jmp	BASIC_START
#elif defined(TARGET_PROTO) /* Prototype hardware. Customisation of this section is expected! */
	tfr	x,d
__ex@0	bra	__ex@0
#else /* Simulator */
	tfr	x,d
	stb	SIM_EXIT_REG
	bra	__exit
#endif

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;
	;;; __start : Entry point to the program
	;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.area	.init
	.globl	__start
__start:

#if defined(HAVE_DIRECT)
	;; Initialize the direct page pointer
	lda	#<s_.direct
	tfr	a,dp
#endif

#if defined(TARGET_COCO)
	;; Turn off interrupts
	orcc #(0x10|0x40)

	;; Setup All RAM Mode
	sta COCO_ALLRAM_MODE
#endif

	;; Initialize the stack. NMI works after this point.
	lds	#__STACK_TOP - 2

	;; Set up hardware; console device, video etc
#if defined(TARGET_PROTO) /* Prototype hardware. Customisation of this section is expected! */
	;;lbsr	_initialise_hardware
#endif

	;; Call any constructors that may be present
	ldu	#s_.ctors
__ctors_loop:
	ldy	,u++
	cmpy	#0
	beq	__ctors_done
	jsr	,y
	bra	__ctors_loop
__ctors_done:

	;; Enable interrupts if not on COCO
#if !defined(TARGET_COCO)
	andcc	#~(0x10|0x40)
#endif

	;; Set up the environment
	ldu	#s_.bss

	;; Set up argc/argv arrays

	;; Call the main function.  The exit code will
	;; be returned in the X register, unless compiled
	;; with -mdret, in which case it comes back in D.
	jsr	_main

	;; Call any destructors
	ldu	#s_.dtors
__dtors_loop:
	ldy	,u++
	cmpy	#0
	beq	__dtors_done
	jsr	,y
	bra	__dtors_loop
__dtors_done:

	;; If main returns, then invoke _*exit() to stop the program
	;; The C library doesn't support -mdret yet, so move the
	;; argument first.
#if defined(__DRET__)
	tfr	d,x
#endif
	;; The newlib exit() is an absolute pig. In calling __call_exitprocs(), it brings with
	;; it the kitchen sink, notably malloc() and friends which are *enormous*.
	jmp	__exit

	;;;
	;;; __crt0_vector : Default handler for interrupts
	;;;
	.area	.text
___crt0_vector:
	;; The default behavior is to simply ignore all
	;; non-reset interrupts.
	rti

	;;;
	;;; vector : The interrupt vector table
	;;; The linker will ensure that this gets loaded at address 0xFFF0.
	;;;
	.area	vector
vectors:
	.word	___crt0_vector
	.word	___crt0_vector
	.word	___crt0_vector
	.word	___crt0_vector
	.word	___crt0_vector
	.word	___crt0_vector
	.word	___crt0_vector
	.word	__start
