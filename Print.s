; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
BP RN 12
DECIMAL EQU 0
LCD_OutDec
	SUB 	SP, SP, #8
	MOV 	R2, #10
	MOV 	BP, SP
	STR 	R2, [BP, #DECIMAL]
	MOV		R4, #0
	PUSH	{LR, R0}
iterate
	LDR	    R2, [BP, #DECIMAL]
	UDIV 	R1, R0, R2
	LDR	    R2, [BP, #DECIMAL]
	MUL 	R1, R1, R2
	SUB 	R3, R0, R1
	ADD		R3, #48
	PUSH 	{R3, R5}
	ADD 	R4, #1
	LDR	    R2, [BP, #DECIMAL]
	UDIV	R0, R0, R2
	ADDS	R0, #0
	BNE     iterate
display	
	POP		{R0, R5}
	BL		ST7735_OutChar
	SUBS	R4, #1
	BNE		display
	POP		{LR, R0}
	ADD     SP, SP, #8
    BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
BIGNUM EQU 9999
LCD_OutFix
	 PUSH {R0, LR}
	 LDR R1, =BIGNUM
	 CMP R0, R1
	 BGT CORNER
	 CMP R0, #0
	 BLT CORNER
	 B	 REGULAR
CORNER
	 LDR R0, =OVEROUT
	 BL ST7735_OutString
	 POP{R0, LR}	 
	 BX   LR

REGULAR
	MOV 	R2, #10
	MOV		R4, #4
iterate2
	UDIV 	R1, R0, R2
	MUL 	R1, R1, R2
	SUB 	R3, R0, R1
	ADD		R3, #48
	PUSH 	{R3, R5}
	UDIV	R0, R0, R2
	ADD		R0, #0
    SUBS 	R4, #1
	BNE     iterate2
putoutfix
	POP		{R0, R5}
	BL		ST7735_OutChar
	MOV		R0, #0x2E
	BL		ST7735_OutChar
	MOV		R4, #3
display2
	POP		{R0, R5}
	BL		ST7735_OutChar
	SUBS	R4, #1
	BNE		display2
	POP		{LR, R0}
    BX  LR

	
OVEROUT
	 DCB '*', '.', '*', '*', '*', 0 
;* * * * * * * * End of LCD_OutFix * * * * * * * *

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
