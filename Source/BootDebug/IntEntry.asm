.686P
.MODEL FLAT
.CODE

; This file implements our basic Int handlers.

IntData			WORD 00h
GlobalData		WORD 00h
IntCallback		DWORD 00000000

public C IntData, IntCallback, GlobalData

; IntCallback needs to be a pointer to a function with this def:
; extern "C" void HandleInterrupt(InterruptContext * Context)

; This sets up the stack for the Callback as well as returns from the interrupt 
Common_Interrupt PROC
	pushad
	
	push gs
	push fs
	push ds
	push es
	
	mov ax, cs:IntData
	mov ds, ax
	mov es, ax
	
	; Make sure we have the global data segment
	mov ax, cs:GlobalData
	mov gs, ax
	
	; Put the pointer in the context on the stack
	mov eax, esp
	push eax
	
	call IntCallback
	
	; Clean up the call
	add esp, 4

	pop es
	pop ds
	pop fs
	pop gs
	
	popad
	
	; Remove the Int and Error Code
	add esp, 8
	iretd

Common_Interrupt ENDP

Default_Interrupt PROC C
	iretd
Default_Interrupt ENDP


; A quick table of all our interrupt handlers so we can build them dynamically.
IntTable:
	DWORD offset int00
	DWORD offset int01
	DWORD offset int02
	DWORD offset int03
	DWORD offset int04
	DWORD offset int05
	DWORD offset int06
	DWORD offset int07
	DWORD offset int08
	DWORD offset int09
	DWORD offset int0A
	DWORD offset int0B
	DWORD offset int0C
	DWORD offset int0D
	DWORD offset int0E
	DWORD offset int0F

	DWORD offset int10
	DWORD offset int11
	DWORD offset int12
	DWORD offset int13
	DWORD offset int14
	DWORD offset int15
	DWORD offset int16
	DWORD offset int17
	DWORD offset int18
	DWORD offset int19
	DWORD offset int1A
	DWORD offset int1B
	DWORD offset int1C
	DWORD offset int1D
	DWORD offset int1E
	DWORD offset int1F

	DWORD offset int20
	DWORD offset int21
	DWORD offset int22
	DWORD offset int23
	DWORD offset int24
	DWORD offset int25
	DWORD offset int26
	DWORD offset int27
	DWORD offset int28
	DWORD offset int29
	DWORD offset int2A
	DWORD offset int2B
	DWORD offset int2C
	DWORD offset int2D
	DWORD offset int2E
	DWORD offset int2F

	DWORD offset int30
	DWORD offset int31
	DWORD offset int32
	DWORD offset int33
	DWORD offset int34
	DWORD offset int35
	DWORD offset int36
	DWORD offset int37
	DWORD offset int38
	DWORD offset int39
	DWORD offset int3A
	DWORD offset int3B
	DWORD offset int3C
	DWORD offset int3D
	DWORD offset int3E
	DWORD offset int3F

	DWORD offset int40
	DWORD offset int41
	DWORD offset int42
	DWORD offset int43
	DWORD offset int44
	DWORD offset int45
	DWORD offset int46
	DWORD offset int47
	DWORD offset int48
	DWORD offset int49
	DWORD offset int4A
	DWORD offset int4B
	DWORD offset int4C
	DWORD offset int4D
	DWORD offset int4E
	DWORD offset int4F

	DWORD offset int50
	DWORD offset int51
	DWORD offset int52
	DWORD offset int53
	DWORD offset int54
	DWORD offset int55
	DWORD offset int56
	DWORD offset int57
	DWORD offset int58
	DWORD offset int59
	DWORD offset int5A
	DWORD offset int5B
	DWORD offset int5C
	DWORD offset int5D
	DWORD offset int5E
	DWORD offset int5F

	DWORD offset int60
	DWORD offset int61
	DWORD offset int62
	DWORD offset int63
	DWORD offset int64
	DWORD offset int65
	DWORD offset int66
	DWORD offset int67
	DWORD offset int68
	DWORD offset int69
	DWORD offset int6A
	DWORD offset int6B
	DWORD offset int6C
	DWORD offset int6D
	DWORD offset int6E
	DWORD offset int6F

	DWORD offset int70
	DWORD offset int71
	DWORD offset int72
	DWORD offset int73
	DWORD offset int74
	DWORD offset int75
	DWORD offset int76
	DWORD offset int77
	DWORD offset int78
	DWORD offset int79
	DWORD offset int7A
	DWORD offset int7B
	DWORD offset int7C
	DWORD offset int7D
	DWORD offset int7E
	DWORD offset int7F

	DWORD offset int80
	DWORD offset int81
	DWORD offset int82
	DWORD offset int83
	DWORD offset int84
	DWORD offset int85
	DWORD offset int86
	DWORD offset int87
	DWORD offset int88
	DWORD offset int89
	DWORD offset int8A
	DWORD offset int8B
	DWORD offset int8C
	DWORD offset int8D
	DWORD offset int8E
	DWORD offset int8F

	DWORD offset int90
	DWORD offset int91
	DWORD offset int92
	DWORD offset int93
	DWORD offset int94
	DWORD offset int95
	DWORD offset int96
	DWORD offset int97
	DWORD offset int98
	DWORD offset int99
	DWORD offset int9A
	DWORD offset int9B
	DWORD offset int9C
	DWORD offset int9D
	DWORD offset int9E
	DWORD offset int9F

	DWORD offset intA0
	DWORD offset intA1
	DWORD offset intA2
	DWORD offset intA3
	DWORD offset intA4
	DWORD offset intA5
	DWORD offset intA6
	DWORD offset intA7
	DWORD offset intA8
	DWORD offset intA9
	DWORD offset intAA
	DWORD offset intAB
	DWORD offset intAC
	DWORD offset intAD
	DWORD offset intAE
	DWORD offset intAF

	DWORD offset intB0
	DWORD offset intB1
	DWORD offset intB2
	DWORD offset intB3
	DWORD offset intB4
	DWORD offset intB5
	DWORD offset intB6
	DWORD offset intB7
	DWORD offset intB8
	DWORD offset intB9
	DWORD offset intBA
	DWORD offset intBB
	DWORD offset intBC
	DWORD offset intBD
	DWORD offset intBE
	DWORD offset intBF

	DWORD offset intC0
	DWORD offset intC1
	DWORD offset intC2
	DWORD offset intC3
	DWORD offset intC4
	DWORD offset intC5
	DWORD offset intC6
	DWORD offset intC7
	DWORD offset intC8
	DWORD offset intC9
	DWORD offset intCA
	DWORD offset intCB
	DWORD offset intCC
	DWORD offset intCD
	DWORD offset intCE
	DWORD offset intCF

	DWORD offset intD0
	DWORD offset intD1
	DWORD offset intD2
	DWORD offset intD3
	DWORD offset intD4
	DWORD offset intD5
	DWORD offset intD6
	DWORD offset intD7
	DWORD offset intD8
	DWORD offset intD9
	DWORD offset intDA
	DWORD offset intDB
	DWORD offset intDC
	DWORD offset intDD
	DWORD offset intDE
	DWORD offset intDF

	DWORD offset intE0
	DWORD offset intE1
	DWORD offset intE2
	DWORD offset intE3
	DWORD offset intE4
	DWORD offset intE5
	DWORD offset intE6
	DWORD offset intE7
	DWORD offset intE8
	DWORD offset intE9
	DWORD offset intEA
	DWORD offset intEB
	DWORD offset intEC
	DWORD offset intED
	DWORD offset intEE
	DWORD offset intEF

	DWORD offset intF0
	DWORD offset intF1
	DWORD offset intF2
	DWORD offset intF3
	DWORD offset intF4
	DWORD offset intF5
	DWORD offset intF6
	DWORD offset intF7
	DWORD offset intF8
	DWORD offset intF9
	DWORD offset intFA
	DWORD offset intFB
	DWORD offset intFC
	DWORD offset intFD
	DWORD offset intFE
	DWORD offset intFF

	DWORD 0

public C IntTable

; Now we have one function for each interrupt. 99% of them do the same thing but if we want to know the int number we have to do it.

; Define the handlers for CPU Traps/Faults
; Division By Zero 
; The only reason this is a function is so it show up in the map file
int00 proc
	push 0
	push 00h
	jmp Common_Interrupt
int00 endp

; Debug Exception
int01:
	push 0
	push 01h
	jmp Common_Interrupt

; Non-Maskable Interrupt
int02:
	push 0
	push 02h
	jmp Common_Interrupt

; Breakpoint
int03:
	push 0
	push 03h
	jmp Common_Interrupt

; Overflow
int04:
	push 0
	push 04h
	jmp Common_Interrupt

; Out of Bounds
int05: 
	push 0
	push 05h
	jmp Common_Interrupt

; Invalid Opcode
int06:
	push 0
	push 06h
	jmp Common_Interrupt

; FPU not Available
int07:
	push 0
	push 07h
	jmp Common_Interrupt

; Double Fault
int08:
	; Has a real error code
	;push 0 
	push 08h 
	jmp Common_Interrupt

; Coprocessor Segment Overrun
int09:
	push 0
	push 09h
	jmp Common_Interrupt

; Invalid TSS
int0A:
	; Has a real error code
	;push 0
	push 0Ah
	jmp Common_Interrupt

; Segment not Present
int0B:
	; Has a real error code
	;push 0 
	push 0Bh
	jmp Common_Interrupt

; Stack Exception
int0C:
	; Has a real error code
	;push 0
	push 0Ch
	jmp Common_Interrupt

; GPF
int0D:
	; Has a real error code
	;push 0
	push 0Dh
	jmp Common_Interrupt

; Page Fault
int0E:
	; Has a real error code
	;push 0
	push 0Eh
	jmp Common_Interrupt

; Reserved
int0F:
	push 0
	push 0Fh
	jmp Common_Interrupt

; FPU Error
int10:
	push 0
	push 10h
	jmp Common_Interrupt

; Alignment Check
int11:
	push 0
	push 11h
	jmp Common_Interrupt

; Machine Check
int12:
	push 0
	push 12h
	jmp Common_Interrupt

; SIMD Error
int13:
	push 0
	push 13h
	jmp Common_Interrupt

; Reserved
int14:
	push 0
	push 14h
	jmp Common_Interrupt

; Reserved
int15:
	push 0
	push 15h
	jmp Common_Interrupt

; Reserved
int16:
	push 0
	push 16h
	jmp Common_Interrupt

; Reserved
int17:
	push 0
	push 17h
	jmp Common_Interrupt

; Reserved
int18:
	; Has a real error code
	;push 0 
	push 18h 
	jmp Common_Interrupt

; Reserved
int19:
	push 0
	push 19h
	jmp Common_Interrupt

; Reserved
int1A:
	push 0
	push 1Ah
	jmp Common_Interrupt

; Reserved
int1B:
	push 0 
	push 1Bh
	jmp Common_Interrupt

; Reserved
int1C:
	push 0
	push 1Ch
	jmp Common_Interrupt

; Reserved
int1D:
	push 0
	push 1Dh
	jmp Common_Interrupt

; Security Exception
int1E:
	; Has a real error code
	;push 0
	push 1Eh
	jmp Common_Interrupt

; Reserved
int1F:
	push 0
	push 1Fh
	jmp Common_Interrupt

int20:	
	push 0
	push 20h
	jmp Common_Interrupt

int21:

	push 0
	push 21h
	jmp Common_Interrupt
	
int22:

	push 0
	push 22h
	jmp Common_Interrupt	

int23:

	push 0
	push 23h
	jmp Common_Interrupt

int24:

	push 0
	push 24h
	jmp Common_Interrupt
	
int25:

	push 0
	push 25h
	jmp Common_Interrupt		
	
int26:

	push 0
	push 26h
	jmp Common_Interrupt

int27:

	push 0
	push 27h
	jmp Common_Interrupt
	
int28:

	push 0
	push 28h
	jmp Common_Interrupt	

int29:

	push 0
	push 29h
	jmp Common_Interrupt

int2A:

	push 0
	push 2Ah
	jmp Common_Interrupt
	
int2B:

	push 0
	push 2Bh
	jmp Common_Interrupt	

int2C:

	push 0
	push 2Ch
	jmp Common_Interrupt	
	
int2D:

	push 0
	push 2Dh
	jmp Common_Interrupt	
	
int2E:

	push 0
	push 2Eh
	jmp Common_Interrupt	
	
int2F:

	push 0
	push 2Fh
	jmp Common_Interrupt	

int30:

	push 0
	push 030h
	jmp Common_Interrupt
int31:

	push 0
	push 031h
	jmp Common_Interrupt
int32:

	push 0
	push 032h
	jmp Common_Interrupt
int33:

	push 0
	push 033h
	jmp Common_Interrupt
int34:

	push 0
	push 034h
	jmp Common_Interrupt
int35:

	push 0
	push 035h
	jmp Common_Interrupt
int36:

	push 0
	push 036h
	jmp Common_Interrupt
int37:

	push 0
	push 037h
	jmp Common_Interrupt
int38:

	push 0
	push 038h
	jmp Common_Interrupt
int39:

	push 0
	push 039h
	jmp Common_Interrupt
int3A:

	push 0
	push 03Ah
	jmp Common_Interrupt
int3B:

	push 0
	push 03Bh
	jmp Common_Interrupt
int3C:

	push 0
	push 03Ch
	jmp Common_Interrupt
int3D:

	push 0
	push 03Dh
	jmp Common_Interrupt
int3E:

	push 0
	push 03Eh
	jmp Common_Interrupt
int3F:

	push 0
	push 03Fh
	jmp Common_Interrupt
int40:

	push 0
	push 040h
	jmp Common_Interrupt
int41:

	push 0
	push 041h
	jmp Common_Interrupt
int42:

	push 0
	push 042h
	jmp Common_Interrupt
int43:

	push 0
	push 043h
	jmp Common_Interrupt
int44:

	push 0
	push 044h
	jmp Common_Interrupt
int45:

	push 0
	push 045h
	jmp Common_Interrupt
int46:

	push 0
	push 046h
	jmp Common_Interrupt
int47:

	push 0
	push 047h
	jmp Common_Interrupt
int48:

	push 0
	push 048h
	jmp Common_Interrupt
int49:

	push 0
	push 049h
	jmp Common_Interrupt
int4A:

	push 0
	push 04Ah
	jmp Common_Interrupt
int4B:

	push 0
	push 04Bh
	jmp Common_Interrupt
int4C:

	push 0
	push 04Ch
	jmp Common_Interrupt
int4D:

	push 0
	push 04Dh
	jmp Common_Interrupt
int4E:

	push 0
	push 04Eh
	jmp Common_Interrupt
int4F:

	push 0
	push 04Fh
	jmp Common_Interrupt
int50:

	push 0
	push 050h
	jmp Common_Interrupt
int51:

	push 0
	push 051h
	jmp Common_Interrupt
int52:

	push 0
	push 052h
	jmp Common_Interrupt
int53:

	push 0
	push 053h
	jmp Common_Interrupt
int54:

	push 0
	push 054h
	jmp Common_Interrupt
int55:

	push 0
	push 055h
	jmp Common_Interrupt
int56:

	push 0
	push 056h
	jmp Common_Interrupt
int57:

	push 0
	push 057h
	jmp Common_Interrupt
int58:

	push 0
	push 058h
	jmp Common_Interrupt
int59:

	push 0
	push 059h
	jmp Common_Interrupt
int5A:

	push 0
	push 05Ah
	jmp Common_Interrupt
int5B:

	push 0
	push 05Bh
	jmp Common_Interrupt
int5C:

	push 0
	push 05Ch
	jmp Common_Interrupt
int5D:

	push 0
	push 05Dh
	jmp Common_Interrupt
int5E:

	push 0
	push 05Eh
	jmp Common_Interrupt
int5F:

	push 0
	push 05Fh
	jmp Common_Interrupt
int60:

	push 0
	push 060h
	jmp Common_Interrupt
int61:

	push 0
	push 061h
	jmp Common_Interrupt
int62:

	push 0
	push 062h
	jmp Common_Interrupt
int63:

	push 0
	push 063h
	jmp Common_Interrupt
int64:

	push 0
	push 064h
	jmp Common_Interrupt
int65:

	push 0
	push 065h
	jmp Common_Interrupt
int66:

	push 0
	push 066h
	jmp Common_Interrupt
int67:

	push 0
	push 067h
	jmp Common_Interrupt
int68:

	push 0
	push 068h
	jmp Common_Interrupt
int69:

	push 0
	push 069h
	jmp Common_Interrupt
int6A:

	push 0
	push 06Ah
	jmp Common_Interrupt
int6B:

	push 0
	push 06Bh
	jmp Common_Interrupt
int6C:

	push 0
	push 06Ch
	jmp Common_Interrupt
int6D:

	push 0
	push 06Dh
	jmp Common_Interrupt
int6E:

	push 0
	push 06Eh
	jmp Common_Interrupt
int6F:

	push 0
	push 06Fh
	jmp Common_Interrupt
int70:

	push 0
	push 070h
	jmp Common_Interrupt
int71:

	push 0
	push 071h
	jmp Common_Interrupt
int72:

	push 0
	push 072h
	jmp Common_Interrupt
int73:

	push 0
	push 073h
	jmp Common_Interrupt
int74:

	push 0
	push 074h
	jmp Common_Interrupt
int75:

	push 0
	push 075h
	jmp Common_Interrupt
int76:

	push 0
	push 076h
	jmp Common_Interrupt
int77:

	push 0
	push 077h
	jmp Common_Interrupt
int78:

	push 0
	push 078h
	jmp Common_Interrupt
int79:

	push 0
	push 079h
	jmp Common_Interrupt
int7A:

	push 0
	push 07Ah
	jmp Common_Interrupt
int7B:

	push 0
	push 07Bh
	jmp Common_Interrupt
int7C:

	push 0
	push 07Ch
	jmp Common_Interrupt
int7D:

	push 0
	push 07Dh
	jmp Common_Interrupt
int7E:

	push 0
	push 07Eh
	jmp Common_Interrupt
int7F:

	push 0
	push 07Fh
	jmp Common_Interrupt
int80:

	push 0
	push 080h
	jmp Common_Interrupt
int81:

	push 0
	push 081h
	jmp Common_Interrupt
int82:

	push 0
	push 082h
	jmp Common_Interrupt
int83:

	push 0
	push 083h
	jmp Common_Interrupt
int84:

	push 0
	push 084h
	jmp Common_Interrupt
int85:

	push 0
	push 085h
	jmp Common_Interrupt
int86:

	push 0
	push 086h
	jmp Common_Interrupt
int87:

	push 0
	push 087h
	jmp Common_Interrupt
int88:

	push 0
	push 088h
	jmp Common_Interrupt
int89:

	push 0
	push 089h
	jmp Common_Interrupt
int8A:

	push 0
	push 08Ah
	jmp Common_Interrupt
int8B:

	push 0
	push 08Bh
	jmp Common_Interrupt
int8C:

	push 0
	push 08Ch
	jmp Common_Interrupt
int8D:

	push 0
	push 08Dh
	jmp Common_Interrupt
int8E:

	push 0
	push 08Eh
	jmp Common_Interrupt
int8F:

	push 0
	push 08Fh
	jmp Common_Interrupt
int90:

	push 0
	push 090h
	jmp Common_Interrupt
int91:

	push 0
	push 091h
	jmp Common_Interrupt
int92:

	push 0
	push 092h
	jmp Common_Interrupt
int93:

	push 0
	push 093h
	jmp Common_Interrupt
int94:

	push 0
	push 094h
	jmp Common_Interrupt
int95:

	push 0
	push 095h
	jmp Common_Interrupt
int96:

	push 0
	push 096h
	jmp Common_Interrupt
int97:

	push 0
	push 097h
	jmp Common_Interrupt
int98:

	push 0
	push 098h
	jmp Common_Interrupt
int99:

	push 0
	push 099h
	jmp Common_Interrupt
int9A:

	push 0
	push 09Ah
	jmp Common_Interrupt
int9B:

	push 0
	push 09Bh
	jmp Common_Interrupt
int9C:

	push 0
	push 09Ch
	jmp Common_Interrupt
int9D:

	push 0
	push 09Dh
	jmp Common_Interrupt
int9E:

	push 0
	push 09Eh
	jmp Common_Interrupt
int9F:

	push 0
	push 09Fh
	jmp Common_Interrupt
intA0:

	push 0
	push 0A0h
	jmp Common_Interrupt
intA1:

	push 0
	push 0A1h
	jmp Common_Interrupt
intA2:

	push 0
	push 0A2h
	jmp Common_Interrupt
intA3:

	push 0
	push 0A3h
	jmp Common_Interrupt
intA4:

	push 0
	push 0A4h
	jmp Common_Interrupt
intA5:

	push 0
	push 0A5h
	jmp Common_Interrupt
intA6:

	push 0
	push 0A6h
	jmp Common_Interrupt
intA7:

	push 0
	push 0A7h
	jmp Common_Interrupt
intA8:

	push 0
	push 0A8h
	jmp Common_Interrupt
intA9:

	push 0
	push 0A9h
	jmp Common_Interrupt
intAA:

	push 0
	push 0AAh
	jmp Common_Interrupt
intAB:

	push 0
	push 0ABh
	jmp Common_Interrupt
intAC:

	push 0
	push 0ACh
	jmp Common_Interrupt
intAD:

	push 0
	push 0ADh
	jmp Common_Interrupt
intAE:

	push 0
	push 0AEh
	jmp Common_Interrupt
intAF:

	push 0
	push 0AFh
	jmp Common_Interrupt
intB0:

	push 0
	push 0B0h
	jmp Common_Interrupt
intB1:

	push 0
	push 0B1h
	jmp Common_Interrupt
intB2:

	push 0
	push 0B2h
	jmp Common_Interrupt
intB3:

	push 0
	push 0B3h
	jmp Common_Interrupt
intB4:

	push 0
	push 0B4h
	jmp Common_Interrupt
intB5:

	push 0
	push 0B5h
	jmp Common_Interrupt
intB6:

	push 0
	push 0B6h
	jmp Common_Interrupt
intB7:

	push 0
	push 0B7h
	jmp Common_Interrupt
intB8:

	push 0
	push 0B8h
	jmp Common_Interrupt
intB9:

	push 0
	push 0B9h
	jmp Common_Interrupt
intBA:

	push 0
	push 0BAh
	jmp Common_Interrupt
intBB:

	push 0
	push 0BBh
	jmp Common_Interrupt
intBC:

	push 0
	push 0BCh
	jmp Common_Interrupt
intBD:

	push 0
	push 0BDh
	jmp Common_Interrupt
intBE:

	push 0
	push 0BEh
	jmp Common_Interrupt
intBF:

	push 0
	push 0BFh
	jmp Common_Interrupt
intC0:

	push 0
	push 0C0h
	jmp Common_Interrupt
intC1:

	push 0
	push 0C1h
	jmp Common_Interrupt
intC2:

	push 0
	push 0C2h
	jmp Common_Interrupt
intC3:

	push 0
	push 0C3h
	jmp Common_Interrupt
intC4:

	push 0
	push 0C4h
	jmp Common_Interrupt
intC5:

	push 0
	push 0C5h
	jmp Common_Interrupt
intC6:

	push 0
	push 0C6h
	jmp Common_Interrupt
intC7:

	push 0
	push 0C7h
	jmp Common_Interrupt
intC8:

	push 0
	push 0C8h
	jmp Common_Interrupt
intC9:

	push 0
	push 0C9h
	jmp Common_Interrupt
intCA:

	push 0
	push 0CAh
	jmp Common_Interrupt
intCB:

	push 0
	push 0CBh
	jmp Common_Interrupt
intCC:

	push 0
	push 0CCh
	jmp Common_Interrupt
intCD:

	push 0
	push 0CDh
	jmp Common_Interrupt
intCE:

	push 0
	push 0CEh
	jmp Common_Interrupt
intCF:

	push 0
	push 0CFh
	jmp Common_Interrupt
intD0:

	push 0
	push 0D0h
	jmp Common_Interrupt
intD1:

	push 0
	push 0D1h
	jmp Common_Interrupt
intD2:

	push 0
	push 0D2h
	jmp Common_Interrupt
intD3:

	push 0
	push 0D3h
	jmp Common_Interrupt
intD4:

	push 0
	push 0D4h
	jmp Common_Interrupt
intD5:

	push 0
	push 0D5h
	jmp Common_Interrupt
intD6:

	push 0
	push 0D6h
	jmp Common_Interrupt
intD7:

	push 0
	push 0D7h
	jmp Common_Interrupt
intD8:

	push 0
	push 0D8h
	jmp Common_Interrupt
intD9:

	push 0
	push 0D9h
	jmp Common_Interrupt
intDA:

	push 0
	push 0DAh
	jmp Common_Interrupt
intDB:

	push 0
	push 0DBh
	jmp Common_Interrupt
intDC:

	push 0
	push 0DCh
	jmp Common_Interrupt
intDD:

	push 0
	push 0DDh
	jmp Common_Interrupt
intDE:

	push 0
	push 0DEh
	jmp Common_Interrupt
intDF:

	push 0
	push 0DFh
	jmp Common_Interrupt
intE0:

	push 0
	push 0E0h
	jmp Common_Interrupt
intE1:

	push 0
	push 0E1h
	jmp Common_Interrupt
intE2:

	push 0
	push 0E2h
	jmp Common_Interrupt
intE3:

	push 0
	push 0E3h
	jmp Common_Interrupt
intE4:

	push 0
	push 0E4h
	jmp Common_Interrupt
intE5:

	push 0
	push 0E5h
	jmp Common_Interrupt
intE6:

	push 0
	push 0E6h
	jmp Common_Interrupt
intE7:

	push 0
	push 0E7h
	jmp Common_Interrupt
intE8:

	push 0
	push 0E8h
	jmp Common_Interrupt
intE9:

	push 0
	push 0E9h
	jmp Common_Interrupt
intEA:

	push 0
	push 0EAh
	jmp Common_Interrupt
intEB:

	push 0
	push 0EBh
	jmp Common_Interrupt
intEC:

	push 0
	push 0ECh
	jmp Common_Interrupt
intED:

	push 0
	push 0EDh
	jmp Common_Interrupt
intEE:

	push 0
	push 0EEh
	jmp Common_Interrupt
intEF:

	push 0
	push 0EFh
	jmp Common_Interrupt
intF0:

	push 0
	push 0F0h
	jmp Common_Interrupt
intF1:

	push 0
	push 0F1h
	jmp Common_Interrupt
intF2:

	push 0
	push 0F2h
	jmp Common_Interrupt
intF3:

	push 0
	push 0F3h
	jmp Common_Interrupt
intF4:

	push 0
	push 0F4h
	jmp Common_Interrupt
intF5:

	push 0
	push 0F5h
	jmp Common_Interrupt
intF6:

	push 0
	push 0F6h
	jmp Common_Interrupt
intF7:

	push 0
	push 0F7h
	jmp Common_Interrupt
intF8:

	push 0
	push 0F8h
	jmp Common_Interrupt
intF9:

	push 0
	push 0F9h
	jmp Common_Interrupt
intFA:

	push 0
	push 0FAh
	jmp Common_Interrupt
intFB:

	push 0
	push 0FBh
	jmp Common_Interrupt
intFC:

	push 0
	push 0FCh
	jmp Common_Interrupt
intFD:

	push 0
	push 0FDh
	jmp Common_Interrupt
intFE:

	push 0
	push 0FEh
	jmp Common_Interrupt

; The only reason this is a function is so it show up in the map file
intFF proc
	push 0
	push 0FFh
	jmp Common_Interrupt
intFF endp

END