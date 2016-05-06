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

;extern "C" uint64_t ReadXCR0();
ReadXCR0 PROC C
	mov ecx, 0
	XGETBV
	ret
ReadXCR0 ENDP


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

;extern "C" uint64_t WriteXCR0(uint62_t Value);
WriteXCR0 PROC C
	mov ecx, 0
	mov edx, [esp + 8]
	mov eax, [esp + 4]

	XSETBV
	ret
WriteXCR0 ENDP

ASSUME FS:Nothing
ASSUME GS:Nothing

;extern "C" uint32_t ReadFS(uint32_t Offset);
ReadFS PROC C
	mov eax, [esp + 4]
	mov eax, fs:[eax]
	ret
ReadFS ENDP

;extern "C" uint32_t ReadGS(uint32_t Offset);
ReadGS PROC C
	mov eax, [esp + 4]
	mov eax, gs:[eax]
	ret
ReadGS ENDP


;extern "C" void ReadCPUID(uint32_t Value, uint32_t Value2, Registers *Result);
ReadCPUID PROC C
	push ebp
	mov ebp, esp

	push ebx
	push esi
	push edi

	mov eax, [ebp + 8]
	mov ecx, [ebp + 12]
	
	mov ebp, [ebp + 16]

	cpuid

	mov [ebp +  0], eax ; EAX
	mov [ebp +  4], ebx ; EBX
	mov [ebp +  8], ecx ; ECX
	mov [ebp + 12], edx ; EDX
	mov [ebp + 16], esi ; ESI
	mov [ebp + 20], edi ; EDI

	pushfd
	pop eax
	mov [ebp + 24], eax ; EFlags

	pop edi
	pop esi
	pop ebx
	
	pop ebp
	ret
ReadCPUID ENDP

;extern "C" uint64_t ReadMSR(uint32_t Register);
ReadMSR PROC C
	push ebp
	mov ebp, esp

	mov ecx, [ebp + 8]

	RDMSR
	
	pop ebp
	ret
ReadMSR ENDP

;extern "C" void WriteMSR(uint32_t Register, uint64_t Value);
WriteMSR PROC C
	push ebp
	mov ebp, esp

	mov edx, [ebp + 16]
	mov eax, [ebp + 12]
	mov ecx, [ebp + 8]

	WRMSR
	
	pop ebp
	ret
WriteMSR ENDP

;extern "C" void FireInt(uint32_t IntNum, Registers *Result);
FireInt PROC C
	; Set up esi to have the address of the results
	push ebp
	mov ebp, esp

	pushad

	mov al, byte ptr [ebp + 8]

	; just for fun we are going to modify our own code!
	mov byte ptr [val], al

	mov ebp, [ebp + 12]

	mov eax, [ebp +  0]; EAX
	mov ebx, [ebp +  4]; EBX
	mov ecx, [ebp +  8]; ECX
	mov edx, [ebp + 12]; EDX
	mov esi, [ebp + 16]; ESI
	mov edi, [ebp + 20]; EDI

	;push [esi +  24]; EFlags
	;popfd

	db 0CDh
val db 003h

	mov [ebp +  0], eax ; EAX
	mov [ebp +  4], ebx ; EBX
	mov [ebp +  8], ecx ; ECX
	mov [ebp + 12], edx ; EDX
	mov [ebp + 16], esi ; ESI
	mov [ebp + 20], edi ; EDI

	pushfd
	pop eax
	mov [ebp + 24], eax ; EFlags

	popad
	pop ebp
	ret
FireInt ENDP

;extern "C" void CallService(uint32_t ServiceAddress, Registers *Result);
CallService PROC C
	push ebp
	mov ebp, esp

	pushad

	push ebp

	mov ebp, [ebp + 12]

	mov eax, [ebp +  0]; EAX
	mov ebx, [ebp +  4]; EBX
	mov ecx, [ebp +  8]; ECX
	mov edx, [ebp + 12]; EDX
	mov esi, [ebp + 16]; ESI
	mov edi, [ebp + 20]; EDI

	;push [esi +  24]; EFlags
	;popfd

	pop ebp

	; This is supposed to be a far call
	push cs
	call dword ptr [ebp + 8]	

	mov ebp, [ebp + 12]
	
	mov [ebp +  0], eax ; EAX
	mov [ebp +  4], ebx ; EBX
	mov [ebp +  8], ecx ; ECX
	mov [ebp + 12], edx ; EDX
	mov [ebp + 16], esi ; ESI
	mov [ebp + 20], edi ; EDI

	pushfd
	pop eax
	mov [ebp + 24], eax ; EFlags

	popad
	pop ebp
	ret

CallService ENDP

;extern "C" uint16_t SetLDT(uint16_t Selector);
SetLDT PROC C
	xor eax, eax
	sldt ax
	mov dx, [esp + 4]
	lldt dx
	ret
SetLDT ENDP

;extern "C" uint16_t SetTR(uint16_t Selector);
SetTR PROC C
	xor eax, eax
	str ax
	mov dx, [esp + 4]
	ltr dx
	ret
SetTR ENDP

;extern "C" void StartV86(uint16_t CS, uint16_t IP, uint16_t SS, uint16_t SP);
StartV86 PROC C
	push ebp
	mov ebp, esp
		
	; Calculate the address of the stack
	mov esi, [ebp + 16] ; SS
	shl esi, 4
	add esi, [ebp + 20] ; SP
	
	; swap stacks
	mov esp, esi

	; Save the address of the thread data
	push ds
	push dword ptr FS:[8]
	
	; The old stack address
	push ss
	mov esi, ebp
	push esi

	; And the return address
	push cs
	push OFFSET Callback

	; Build the v86 context

	push 0 ; GS
	push 0 ; FS
	push 0 ; DS
	push 0 ; ES
	push [ebp + 16] ; SS
	mov eax, [ebp + 20] ; SP
	sub eax, 24
	push eax

	pushd 00020002h ; EFlags with VM bit set 

	push [ebp + 8] ; CS
	push [ebp + 12] ; IP
	iretd

Callback:
	pop ebp
	ret

StartV86 ENDP



END