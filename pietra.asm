BITS 64
segment .text
	global _start:
dump:
	push  rbp
	mov   rbp, rsp
	sub   rsp, 64
	mov   QWORD  [rbp-56], rdi
	mov   QWORD  [rbp-8], 1
	mov   eax, 32
	sub   rax, QWORD  [rbp-8]
	mov   BYTE  [rbp-48+rax], 10
.L2:
	mov   rcx, QWORD  [rbp-56]
	mov   rdx, -3689348814741910323
	mov   rax, rcx
	mul   rdx
	shr   rdx, 3
	mov   rax, rdx
	sal   rax, 2
	add   rax, rdx
	add   rax, rax
	sub   rcx, rax
	mov   rdx, rcx
	mov   eax, edx
	lea   edx, [rax+48]
	mov   eax, 31
	sub   rax, QWORD  [rbp-8]
	mov   BYTE  [rbp-48+rax], dl
	add   QWORD  [rbp-8], 1
	mov   rax, QWORD  [rbp-56]
	mov   rdx, -3689348814741910323
	mul   rdx
	mov   rax, rdx
	shr   rax, 3
	mov   QWORD  [rbp-56], rax
	cmp   QWORD  [rbp-56], 0
	jne   .L2
	mov   eax, 32
	sub   rax, QWORD  [rbp-8]
	lea   rdx, [rbp-48]
	lea   rcx, [rdx+rax]
	mov   rax, QWORD  [rbp-8]
	mov   rdx, rax
	mov   rsi, rcx
	mov   edi, 1
	mov   rax, 1
	syscall
	nop
	leave
	ret
sbrk:
.PB.sbrk:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
	lea rax, G__heap_end
	mov rax, [rax]
	push rax
.L1:
	mov rax, 0
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
		cmp rax, 0
	je .L0
	lea rax, G__heap_end
	push rax
.L2:
	mov rax, 0
	mov rdi, rax
.L3:
	mov rax, 12
	syscall
	pop rdi
	mov [rdi], rax
.L0:
	lea rax, G__heap_end
	push rax
.L4:
	mov rax, 8
	push rax
	lea rax, G__heap_end
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	lea rax, G__heap_end
	mov rax, [rax]
	mov rdi, rax
.L5:
	mov rax, 12
	syscall
.L6:
	mov rax, 8
	push rax
	lea rax, G__heap_end
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	sub rax, rbx
	jmp .PE.sbrk
	mov rax, 0
.PE.sbrk:
	leave
	ret
strlen:
.PB.strlen:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
.L7:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L8:
.L10:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	movsx rax, BYTE [rax]

		cmp rax, 0
	je .L9
.L11:
	lea rax, QWORD [rbp - 8]
	push rax
.L12:
	mov rax, 1
	push rax
.L13:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	jmp .L8
.L9:
.L14:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	push rax
.L15:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	sub rax, rbx
	jmp .PE.strlen
	mov rax, 0
.PE.strlen:
	leave
	ret
main:
.PB.main:
	push rbp
	mov rbp, rsp
	sub rsp, 40
	lea rax, QWORD [rbp - 8]
	push rax
.L16:
	mov rax, 20
	pop rdi
	mov [rdi], rax
	lea rax, QWORD [rbp - 16]
	push rax
.L17:
	mov rax, 8
	mov rdi, rax
	mov rax, sbrk
	call rax
	pop rdi
	mov [rdi], rax
	lea rax, QWORD [rbp - 24]
	push rax
	lea rax, G__heap_end
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L18:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	push rax
.L19:
	mov rax, 1
	pop rdi
	mov [rdi], rax
.L20:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	mov rax, QWORD [rax]

	mov rdi, rax
	call dump
	lea rax, QWORD [rbp - 32]
	push rax
.L21:
	mov rax, S0
	pop rdi
	mov [rdi], rax
	lea rax, QWORD [rbp - 40]
	push rax
.L22:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdi, rax
	mov rax, strlen
	call rax
	pop rdi
	mov [rdi], rax
.L23:
	mov rax, 1
	mov rdi, rax
.L24:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rsi, rax
.L25:
	lea rax, QWORD [rbp - 40]
	mov rax, QWORD [rax]

	mov rdx, rax
.L26:
	mov rax, 1
	syscall
	mov rax, 0
.PE.main:
	leave
	ret
_start:
	mov rax, 12
	mov rdi, 0
	syscall
	mov [__heap_begin__], rax
	lea rax, Gtrue
	push rax
.L27:
	mov rax, 1
	pop rdi
	mov [rdi], rax
	lea rax, Gfalse
	push rax
.L28:
	mov rax, 0
	pop rdi
	mov [rdi], rax
	lea rax, Gnull
	push rax
.L29:
	mov rax, 0
	pop rdi
	mov [rdi], rax
	call main
	mov rax, 60
	mov rdi, 0
	syscall
segment .data
	S0: db 104, 101, 108, 108, 111, 44, 32, 102, 117, 99, 107, 32, 109, 101, 32, 112, 108, 101, 97, 115, 101, 63, 10, 0
segment .bss
	__heap_begin__: resq 1
	Gtrue: resb 8
	Gfalse: resb 8
	Gnull: resb 8
	G__heap_end: resb 8
