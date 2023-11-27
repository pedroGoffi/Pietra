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
exit:
.PB.exit:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L0:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L1:
	mov rax, 60
	syscall
	mov rax, 0
.PE.exit:
	leave
	ret
read:
.PB.read:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	mov [rbp - 24], rdx
.L2:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L3:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
.L4:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rdx, rax
.L5:
	mov rax, 0
	syscall
	jmp .PE.read
	mov rax, 0
.PE.read:
	leave
	ret
write:
.PB.write:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	mov [rbp - 24], rdx
.L6:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L7:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
.L8:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rdx, rax
.L9:
	mov rax, 1
	syscall
	mov rax, 0
.PE.write:
	leave
	ret
brk:
.PB.brk:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L10:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L11:
	mov rax, 12
	syscall
	jmp .PE.brk
	mov rax, 0
.PE.brk:
	leave
	ret
sbrk:
.PB.sbrk:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
	;; NOTE: inline asm here.

	mov rax, __heap_end__
	pop rdi
	mov [rdi], rax
.L12:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	push rax
.L13:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L14:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	mov rax, brk
	push rax
.L15:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rax, QWORD [rax]

	mov rdi, rax
	pop rax
	call rax
.L16:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L17:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	sub rax, rbx
	jmp .PE.sbrk
	mov rax, 0
.PE.sbrk:
	leave
	ret
open:
.PB.open:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	mov [rbp - 24], rdx
.L18:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L19:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rsi, rax
.L20:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rdx, rax
.L21:
	mov rax, 2
	syscall
	jmp .PE.open
	mov rax, 0
.PE.open:
	leave
	ret
socket:
.PB.socket:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	mov [rbp - 24], rdx
.L22:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L23:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rsi, rax
.L24:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rdx, rax
.L25:
	mov rax, 41
	syscall
	jmp .PE.socket
	mov rax, 0
.PE.socket:
	leave
	ret
accept:
.PB.accept:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	mov [rbp - 24], rdx
.L26:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L27:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
.L28:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rdx, rax
.L29:
	mov rax, 43
	syscall
	jmp .PE.accept
	mov rax, 0
.PE.accept:
	leave
	ret
bind:
.PB.bind:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	mov [rbp - 24], rdx
.L30:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L31:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
.L32:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rdx, rax
.L33:
	mov rax, 49
	syscall
	jmp .PE.bind
	mov rax, 0
.PE.bind:
	leave
	ret
listen:
.PB.listen:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
.L34:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L35:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rsi, rax
.L36:
	mov rax, 50
	syscall
	jmp .PE.listen
	mov rax, 0
.PE.listen:
	leave
	ret
fork:
.PB.fork:
	push rbp
	mov rbp, rsp
.L37:
	mov rax, 57
	syscall
	jmp .PE.fork
	mov rax, 0
.PE.fork:
	leave
	ret
execve:
.PB.execve:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	mov [rbp - 24], rdx
.L38:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L39:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
.L40:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	mov rdx, rax
.L41:
	mov rax, 59
	syscall
	jmp .PE.execve
	mov rax, 0
.PE.execve:
	leave
	ret
close:
.PB.close:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L42:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L43:
	mov rax, 3
	syscall
	jmp .PE.close
	mov rax, 0
.PE.close:
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
.L44:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L45:
.L47:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	cmp rax, 0
	je .L46
.L48:
	lea rax, QWORD [rbp - 16]
	push rax
.L49:
	mov rax, 1
	push rax
.L50:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	jmp .L45
.L46:
.L51:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L52:
	lea rax, QWORD [rbp - 16]
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
std_impl_puts:
.PB.std_impl_puts:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
	mov rax, strlen
	push rax
.L53:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	mov rax, write
	push rax
.L54:
	mov rax, 1
	mov rdi, rax
.L55:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rsi, rax
.L56:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rdx, rax
	pop rax
	call rax
	mov rax, 0
.PE.std_impl_puts:
	leave
	ret
main:
.PB.main:
	push rbp
	mov rbp, rsp
	mov rdi, rax
	mov rax, std_impl_puts
	push rax
.L57:
	mov rax, S0
	mov rdi, rax
	pop rax
	call rax
	mov rax, 0
.PE.main:
	leave
	ret
_start:
	mov rax, 12
	mov rdi, 0
	syscall
	mov [__heap_end__], rax
	lea rax, Gtrue
	push rax
.L58:
	mov rax, 1
	pop rdi
	mov [rdi], rax
	lea rax, Gfalse
	push rax
.L59:
	mov rax, 0
	pop rdi
	mov [rdi], rax
	lea rax, Gnull
	push rax
.L60:
	mov rax, 0
	pop rdi
	mov [rdi], rax
	call main
	mov rax, 60
	mov rdi, 0
	syscall
segment .data
	S0: db 104, 101, 108, 108, 111, 44, 32, 119, 111, 114, 108, 100, 10, 0
segment .bss
	__heap_end__: resq 1
	__va_offset__: resq 1
	Gtrue: resb 8
	Gfalse: resb 8
	Gnull: resb 8
