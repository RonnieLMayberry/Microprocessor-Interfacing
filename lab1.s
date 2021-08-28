	AREA LAB1, CODE, READONLY
		ENTRY
		EXPORT __main
	
__main
		MOV R1, #12	; integer to take factorial of (factorial 12 / 32 bit max, factorial 20 / 64 bit max)
		MOV R2, #1	; register to hold final product
	
EndlessLoop
		MUL R2, R2, R1	; multiply R1 into product register
		SUB R1, R1, #1	; decrement R1

		CMP R1, #0	; check if R1 is non-zero
		BNE EndlessLoop	; if R1 is non-zero, loop again
	
STOP	B	STOP

		END