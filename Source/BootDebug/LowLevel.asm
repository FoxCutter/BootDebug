.686P
.MODEL FLAT
.CODE

JmpAddr DWORD offset FlushCS	
JmpSeg  WORD 0

;extern "C" void SwapSelectors(uint32_t CodeSeg, uint32_t DataSeg);
SwapSelectors PROC C
	mov ax, [esp + 8]
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov ax, [esp + 4]
	mov word ptr [JmpSeg], ax	
	
	jmp fword ptr [JmpAddr]

FlushCS::
	ret

SwapSelectors ENDP

;extern "C" void OutPortb(uint16_t Port, uint8_t Value);
OutPortb PROC C
	mov dx, [esp + 4]
	mov al, byte ptr [esp + 8]
	out dx, al	
	ret
OutPortb ENDP

;extern "C" void OutPortw(uint16_t Port, uint16_t Value);
OutPortw PROC C
	mov dx, [esp + 4]
	mov ax, word ptr [esp + 8]
	out dx, ax
	ret
OutPortw ENDP

;extern "C" void OutPortd(uint16_t Port, uint32_t Value);
OutPortd PROC C
	mov dx, [esp + 4]
	mov eax, [esp + 8]
	out dx, eax
	ret
OutPortd ENDP

;extern "C" uint8_t InPortb(uint16_t Port);
InPortb PROC C
	xor eax, eax
	mov dx, [esp + 4]
	in al, dx
	ret
InPortB ENDP

;extern "C" uint16_t InPortw(uint16_t Port);
InPortw PROC C
	xor eax, eax
	mov dx, [esp + 4]
	in ax, dx
	ret
InPortw ENDP

;extern "C" uint32_t InPortd(uint16_t Port);
InPortd PROC C
	xor eax, eax
	mov dx, [esp + 4]
	in eax, dx
	ret
InPortd ENDP

;extern "C" uint32_t ReadCR0();
ReadCR0 PROC C
	mov eax, cr0
	ret
ReadCR0 ENDP

;extern "C" uint32_t ReadCR2();
ReadCR2 PROC C
	mov eax, cr2
	ret
ReadCR2 ENDP

;extern "C" uint32_t ReadCR3();
ReadCR3 PROC C
	mov eax, cr3
	ret
ReadCR3 ENDP

;extern "C" uint32_t ReadCR4();
ReadCR4 PROC C
	mov eax, cr4
	ret
ReadCR4 ENDP

;extern "C" void WriteCR0(uint32_t Value);
WriteCR0 PROC C
	mov eax, [esp + 4]
	mov cr0, eax
	ret
WriteCR0 ENDP

;extern "C" void WriteCR2(uint32_t Value);
WriteCR2 PROC C
	mov eax, [esp + 4]
	mov CR2, eax
	ret
WriteCR2 ENDP

;extern "C" void WriteCR3(uint32_t Value);
WriteCR3 PROC C
	mov eax, [esp + 4]
	mov CR3, eax
	ret
WriteCR3 ENDP

;extern "C" void WriteCR4(uint32_t Value);
WriteCR4 PROC C
	mov eax, [esp + 4]
	mov CR4, eax
	ret
WriteCR4 ENDP

;extern "C" void ReadCPUID(uint32_t Value, uint32_t Value2, Registers *Result);
ReadCPUID PROC C
	; Set up esi to have the address of the results
	push ebp
	mov ebp, esp

	push esi
	push ebx

	mov eax, [ebp + 8]
	mov ecx, [ebp + 12]
	mov esi, [ebp + 16]

	cpuid

	mov [esi +  0], edi ; EDI
	mov [esi +  4], esi ; ESI
	mov [esi +  8], ebp ; EBP
	mov [esi + 12], esp ; ESP
	mov [esi + 16], ebx ; EBX
	mov [esi + 20], edx ; EDX
	mov [esi + 24], ecx ; ECX
	mov [esi + 28], eax ; EAX

	pop ebx
	pop esi
	
	pop ebp
	ret
ReadCPUID ENDP

;extern "C" void FireInt(uint32_t IntNum, Registers *Result);
FireInt PROC C
	; Set up esi to have the address of the results
	push ebp
	mov ebp, esp

	pushad

	mov al, byte ptr [ebp + 8]
	mov esi, [ebp + 12]

	; just for fun we are going to modify our own code!
	mov byte ptr [val], al

	;mov edi, [esi +  0]; EDI
	;mov esi, [esi +  4]; ESI
	;mov ebp, [esi +  8]; EBP
	;mov esp, [esi + 12]; ESP
	mov ebx, [esi + 16]; EBX
	mov edx, [esi + 20]; EDX
	mov ecx, [esi + 24]; ECX
	mov eax, [esi + 28]; EAX
	
	db 0CDh
val db 003h

	mov [esi +  0], edi ; EDI
	mov [esi +  4], esi ; ESI
	mov [esi +  8], ebp ; EBP
	mov [esi + 12], esp ; ESP
	mov [esi + 16], ebx ; EBX
	mov [esi + 20], edx ; EDX
	mov [esi + 24], ecx ; ECX
	mov [esi + 28], eax ; EAX

	popad
	pop ebp
	ret
FireInt ENDP

END