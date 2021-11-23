IOCON_P0_13	EQU 0x4002C034 ; LED 2 P0[13] IOCON_P0_13 R/W 0x1B0 0x4002C034 A
IOCON_P1_13 	EQU 0x4002C0B4 ; LED 3 P1[13] IOCON_P1_13 R/W 0x030 0x4002C0B4 D
IOCON_P1_18 	EQU 0x4002C0C8 ; LED 1 P1[18] IOCON_P1_18 R/W 0x030 0x4002C0C8 D
IOCON_P2_19	EQU 0x4002C14C ; LED 4 P2[19] IOCON_P2_19 R/W 0x030 0x4002C14C D
	
IOCON_P0_5	EQU 0x4002C014	; external switch - PIN 33
IOCON_P1_2 	EQU 0x4002C088 ; external LED - PIN 30 POSITIVE LOGIC
IOCON_P2_10 	EQU 0x4002C128 ; user switch - PIN 23

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
bit2		EQU (1 <<  2)
bit5		EQU (1 <<  5)
bit10		EQU (1 << 10)
bit13 		EQU (1 << 13) ; 0x00002000
bit18 		EQU (1 << 18) ; 0x00040000
bit19		EQU (1 << 19)
	
		AREA lab3, CODE, READONLY
		ENTRY
		EXPORT 			__main
__main
		; setup ext led on port1_5
		LDR 			R1, =IOCON_P1_2 ; ping 30 extLED
		LDR 			R2, =0x00000000
		STR 			R2,[R1,#0x00]
		
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1
		LDR 			R3,=bit2 ; set LED 1 port pin to output
		STR 			R3,[R1,#DIR_REG_OFFSET] ; set DIRECTION bits
		
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1 
		LDR 			R3,=0x00000000 ; MSK register setting
		STR 			R3,[R1,#MSK_REG_OFFSET] ; set MASK bits 
		
		// positive logic, this should turn LED off
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1 
		LDR 			R3,=bit2 ; MSK register setting
		STR 			R3,[R1,#CLR_REG_OFFSET] ; set MASK bits 
		
		bl delay
		
		// track if LED is on or off
		MOV 			R12,#0 // start off
		
		b read
		
read
		; setup ext switch port 0, p5
		LDR 			R1, =IOCON_P0_5
		LDR 			R2, =0x00000000
		STR 			R2,[R1,#0x00]
		
		LDR 			R1,=LPC4088QSB_P0 ; pointer to base reg of port1
		LDR 			R3,=bit5 ; set LED 1 port pin to output
		STR 			R3,[R1,#DIR_REG_OFFSET] ; set DIRECTION bits
		
		LDR 			R1,=LPC4088QSB_P0 ; pointer to base reg of port1 
		LDR 			R3,=0x00000000 ; MSK register setting
		STR 			R3,[R1,#MSK_REG_OFFSET] ; set MASK bits 
		
		LDR 			R1,=LPC4088QSB_P0 ; pointer to base reg of port1 
		LDR 			R3,=0x00000000 ; MSK register setting
		LDR 			R3,[R1,#PIN_REG_OFFSET] ; set MASK bits 			
		
		CMP			R3, #bit5
		BEQ 			pressed
		
		b read
		
pressed
		CMP 			R12,#0 ; 0 is off
		BEQ			turnon
		BNE			turnoff
		
turnon
		LDR 			R1, =IOCON_P1_2 ; ping 30 extLED
		LDR 			R2, =0x00000000
		STR 			R2,[R1,#0x00]
		
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1
		LDR 			R3,=bit2 ; set LED 1 port pin to output
		STR 			R3,[R1,#DIR_REG_OFFSET] ; set DIRECTION bits
		
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1 
		LDR 			R3,=0x00000000 ; MSK register setting
		STR 			R3,[R1,#MSK_REG_OFFSET] ; set MASK bits 
		
		// positive logic, this should turn LED on
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1 
		LDR 			R3,=bit2 ; MSK register setting
		STR 			R3,[R1,#SET_REG_OFFSET] ; set MASK bits 

		MOV			R12,#1
		
		bl delay
		
		b read
		
turnoff
		LDR 			R1, =IOCON_P1_2 ; ping 30 extLED
		LDR 			R2, =0x00000000
		STR 			R2,[R1,#0x00]
		
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1
		LDR 			R3,=bit2 ; set LED 1 port pin to output
		STR 			R3,[R1,#DIR_REG_OFFSET] ; set DIRECTION bits
		
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1 
		LDR 			R3,=0x00000000 ; MSK register setting
		STR 			R3,[R1,#MSK_REG_OFFSET] ; set MASK bits 
		
		// positive logic, this should turn LED off
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1 
		LDR 			R3,=bit2 ; MSK register setting
		STR 			R3,[R1,#CLR_REG_OFFSET] ; set MASK bits 

		MOV			R12,#0

		bl delay

		b read
		
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
