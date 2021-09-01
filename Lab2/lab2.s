
IOCON_P0_13	EQU 0x4002C034 ; LED 2 P0[13] IOCON_P0_13 R/W 0x1B0 0x4002C034 A
IOCON_P1_13 	EQU 0x4002C0B4 ; LED 3 P1[13] IOCON_P1_13 R/W 0x030 0x4002C0B4 D
IOCON_P1_18 	EQU 0x4002C0C8 ; LED 1 P1[18] IOCON_P1_18 R/W 0x030 0x4002C0C8 D
IOCON_P2_19	EQU 0x4002C14C ; LED 4 P2[19] IOCON_P2_19 R/W 0x030 0x4002C14C D

; This is essentially Table 94 from UM10562
LPC4088QSB_P0 	EQU 0x20098000
LPC4088QSB_P1 	EQU 0x20098020
LPC4088QSB_P2 	EQU 0x20098040
LPC4088QSB_P3 	EQU 0x20098060
LPC4088QSB_P4 	EQU 0x20098080
LPC4088QSB_P5 	EQU 0x200980A0
DIR_REG_OFFSET 	EQU 0x00
MSK_REG_OFFSET 	EQU 0x10
PIN_REG_OFFSET 	EQU 0x14
SET_REG_OFFSET 	EQU 0x18
CLR_REG_OFFSET 	EQU 0x1C
	
; End Table 94
bit13 		EQU (1 << 13) ; 0x00002000
bit18 		EQU (1 << 18) ; 0x00040000
bit19		EQU (1 << 19) ; 0x00800000
	
		AREA lab2, CODE, READONLY
		ENTRY
		EXPORT 			__main
__main
		
EndlessLoop	
		
		; turn all LEDs off 
		; LOGIC : POSITIVE INDICATES SET = ON / CLR = OFF, NEGATIVE INDICATES SET = OFF / CLR = ON
		
		; NEGATIVE LOGIC
		
		; setup LED#1 on port1[18]
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1
		LDR 			R3,=bit18 ; SET register setting
		STR 			R3,[R1,#SET_REG_OFFSET] ; set SET bits - This turns OFF the LED
		
		; setup LED#2 on port0[13]
		LDR 			R1,=LPC4088QSB_P0 ; pointer to base reg of port0
		LDR 			R3,=bit13 ; SET register setting
		STR 			R3,[R1,#SET_REG_OFFSET] ; set SET bits - This turns OFF the LED
		
		; POSITIVE LOGIC
		
		; setup LED#3 on port1[13]
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1
		LDR 			R3,=bit13 ; CLR register setting
		STR 			R3,[R1,#CLR_REG_OFFSET] ; set CLEAR bits - This turns OFF the LED
		
		; setup LED#4 on port2[19]
		LDR 			R1,=LPC4088QSB_P2 ; pointer to base reg of port2
		LDR 			R3,=bit19 ; CLR register setting
		STR 			R3,[R1,#CLR_REG_OFFSET] ; set CLEAR bits - This turns OFF the LED
		
		; ------------------------------------------
		
		; port 1[18] LED 1
		; setup the, IOCON, I/O configuration - default is 0x30, setup to 0x00
		; setup for GPIO and inactive and no hysteresis
		
		LDR 			R1, =IOCON_P1_18
		LDR 			R2, =0x00000000
		STR 			R2,[R1,#0x00]
		
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1
		LDR 			R3,=bit18 ; set LED 1 port pin to output - pin 18
		STR 			R3,[R1,#DIR_REG_OFFSET] ; set DIRECTION bits
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1 
		LDR 			R3,=0x00000000 ; MSK register setting
		STR 			R3,[R1,#MSK_REG_OFFSET] ; set MASK bits 
		
		; turn on LED 1, delay, turn off, delay - NEGATIVE LOGIC
		
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1
		LDR 			R3,=bit18 ; CLR register setting
		STR 			R3,[R1,#CLR_REG_OFFSET] ; set CLEAR bits - This turns ON the LED
		bl delay
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1
		LDR 			R3,=bit18 ; SET register setting
		STR 			R3,[R1,#SET_REG_OFFSET] ; set SET bits - This turns OFF the LED
		bl delay
		
		; port 0[13] LED 2
		; setup the, IOCON, I/O configuration - default is 0x30, setup to 0x00
		; setup for GPIO and inactive and no hysteresis
		
		LDR 			R1, =IOCON_P0_13
		LDR 			R2, =0x00000000
		STR 			R2,[R1,#0x00]
		
		LDR 			R1,=LPC4088QSB_P0 ; pointer to base reg of port0
		LDR 			R3,=bit13 ; set LED 2 port pin to output - pin 13
		STR 			R3,[R1,#DIR_REG_OFFSET] ; set DIRECTION bits
		LDR 			R1,=LPC4088QSB_P0 ; pointer to base reg of port0
		LDR 			R3,=0x00000000 ; MSK register setting
		STR 			R3,[R1,#MSK_REG_OFFSET] ; set MASK bits 
		
		; turn on LED 2, delay, turn off, delay - NEGATIVE LOGIC
		
		LDR 			R1,=LPC4088QSB_P0 ; pointer to base reg of port0
		LDR 			R3,=bit13 ; CLR register setting
		STR 			R3,[R1,#CLR_REG_OFFSET] ; set CLEAR bits - This turns ON the LED
		bl delay
		LDR 			R1,=LPC4088QSB_P0 ; pointer to base reg of port1
		LDR 			R3,=bit13 ; SET register setting
		STR 			R3,[R1,#SET_REG_OFFSET] ; set SET bits - This turns OFF the LED
		bl delay
		
		; port 1[13] LED 3
		; setup the, IOCON, I/O configuration - default is 0x30, setup to 0x00
		; setup for GPIO and inactive and no hysteresis
		
		LDR 			R1, =IOCON_P1_13
		LDR 			R2, =0x00000000
		STR 			R2,[R1,#0x00]
		
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1
		LDR 			R3,=bit13 ; set LED 3 port pin to output - pin 13
		STR 			R3,[R1,#DIR_REG_OFFSET] ; set DIRECTION bits
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1 
		LDR 			R3,=0x00000000 ; MSK register setting
		STR 			R3,[R1,#MSK_REG_OFFSET] ; set MASK bits 
		
		; turn on LED 3, delay, turn off, delay - POSITIVE LOGIC
		
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1
		LDR 			R3,=bit13 ; SET register setting
		STR 			R3,[R1,#SET_REG_OFFSET] ; set SET bits - This turns ON the LED
		bl delay
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1
		LDR 			R3,=bit13 ; CLR register setting
		STR 			R3,[R1,#CLR_REG_OFFSET] ; set CLEAR bits - This turns OFF the LED
		bl delay
		
		; port 2[19] LED 4
		; setup the, IOCON, I/O configuration - default is 0x30, setup to 0x00
		; setup for GPIO and inactive and no hysteresis
		
		LDR 			R1, =IOCON_P2_19
		LDR 			R2, =0x00000000
		STR 			R2,[R1,#0x00]
		
		LDR 			R1,=LPC4088QSB_P2 ; pointer to base reg of port2
		LDR 			R3,=bit19 ; set LED 4 port pin to output - pin 19
		STR 			R3,[R1,#DIR_REG_OFFSET] ; set DIRECTION bits
		LDR 			R1,=LPC4088QSB_P2 ; pointer to base reg of port2
		LDR 			R3,=0x00000000 ; MSK register setting
		STR 			R3,[R1,#MSK_REG_OFFSET] ; set MASK bits 
		
		; turn on LED 4, delay, turn off, delay - POSITIVE LOGIC
		
		LDR 			R1,=LPC4088QSB_P2 ; pointer to base reg of port2
		LDR 			R3,=bit19 ; SET register setting
		STR 			R3,[R1,#SET_REG_OFFSET] ; set SET bits - This turns ON the LED
		bl delay
		LDR 			R1,=LPC4088QSB_P2 ; pointer to base reg of port2
		LDR 			R3,=bit19 ; CLR register setting
		STR 			R3,[R1,#CLR_REG_OFFSET] ; set CLEAR bits - This turns OFF the LED
		bl delay
		
		B EndlessLoop
stp
		b stp
 
; ------------------------------------------
; Delay subroutine 
delay
		PUSH {R0,LR}
		MOVS r0, #0x1000000 ;1 cycle
B1 
		SUBS r0, r0, #1 ;1 cycle
		BNE B1 ;2 if taken, 1 otherwise
		POP {R0,PC}
; end delay subroutine
; ------------------------------------------

	ALIGN
	END
