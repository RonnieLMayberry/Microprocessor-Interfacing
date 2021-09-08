IOCON_P0_13	EQU 0x4002C034 ; LED 2 P0[13] IOCON_P0_13 R/W 0x1B0 0x4002C034 A
IOCON_P1_13 	EQU 0x4002C0B4 ; LED 3 P1[13] IOCON_P1_13 R/W 0x030 0x4002C0B4 D
IOCON_P1_18 	EQU 0x4002C0C8 ; LED 1 P1[18] IOCON_P1_18 R/W 0x030 0x4002C0C8 D
IOCON_P2_19	EQU 0x4002C14C ; LED 4 P2[19] IOCON_P2_19 R/W 0x030 0x4002C14C D
	
IOCON_P1_2 	EQU  0x4002C088
IOCON_P2_10 	EQU  0x4002C128

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
bit10		EQU (1 << 10)
bit13 		EQU (1 << 13) ; 0x00002000
bit18 		EQU (1 << 18) ; 0x00040000
bit19		EQU (1 << 19) ; 0x00800000
	
		AREA lab3, CODE, READONLY
		ENTRY
		EXPORT 			__main
__main
		; port 2[10] LED
		; setup the, IOCON, I/O configuration - default is 0x30, setup to 0x00
		; setup for GPIO and inactive and no hysteresis
		
		LDR 			R1, =IOCON_P1_2
		LDR 			R2, =0x00000000
		STR 			R2,[R1,#0x00]
		
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1
		LDR 			R3,=bit2 ; set LED 1 port pin to output - pin 18
		STR 			R3,[R1,#DIR_REG_OFFSET] ; set DIRECTION bits
		
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port1 
		LDR 			R3,=0x00000000 ; MSK register setting
		STR 			R3,[R1,#MSK_REG_OFFSET] ; set MASK bits 
		
		; turn LED off
		LDR 			R1,=LPC4088QSB_P1 ; pointer to base reg of port2
		LDR 			R3,=bit2 ; 
		STR 			R3,[R1,#SET_REG_OFFSET] ; set CLEAR bits - This turns ON the LED
		bl delay
		
		MOV				R12, #1
		
read		
		LDR 			R5,=LPC4088QSB_P2 ; pointer to base reg of port2
		LDR 			R3,=bit10 

		LDR				R3,[R5,#PIN_REG_OFFSET]
		AND				R4, R4, R3
		CMP				R4, #1
		
		BEQ read
		BNE pressed
		
pressed
		CMP				R12, #1
		BEQ turnoff		;if true, turn off LED '1'
		BNE turnon		;if false, turn on LED '0'
	
turnon
		MOV				R12, #1

		LDR 			R1,=LPC4088QSB_P2 ; pointer to base reg of port2
		LDR 			R3,=bit10 ; 
		STR 			R3,[R1,#SET_REG_OFFSET] ; set CLEAR bits - This turns OFF the LED
		bl delay
		
		b read		

turnoff
		MOV				R12, #0

		LDR 			R1,=LPC4088QSB_P2 ; pointer to base reg of port2
		LDR 			R3,=bit10 ; 
		STR 			R3,[R1,#CLR_REG_OFFSET] ; set CLEAR bits - This turns OFF the LED
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
