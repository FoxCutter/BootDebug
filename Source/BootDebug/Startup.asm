.686P
.MODEL FLAT
.CODE

; extern "C" void MultiBootMain(void *Address, uint32_t Magic)
EXTERNDEF C MultiBootMain:PROC

; 32k stack size
StackSize EQU 8000h

MultiBootEntry Proc C public
	; Set up the stack
	mov		esp, (offset MyStack + StackSize)

	; Clear out the EFLAGS
	xor		ecx, ecx
	push	ecx
	popfd	

	; Just to be doubly sure that interrupts are off.
	cli
	
	; Call the entry point
	push	eax			; Signature
	push	ebx			; MB Data Address
	call	MultiBootMain

	; if we return (which we shouldn't) bail out
ExitLoop:
	hlt
	jmp ExitLoop

MultiBootEntry EndP

.data
MyStack byte StackSize dup(?)

END