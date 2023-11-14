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
strlen:
.PB.strlen:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
.L0:
	mov rax, 0
	pop rdi
	mov [rdi], rax
.L1:
.L3:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	cmp rax, 0
	je .L2
.L4:
	lea rax, QWORD [rbp - 16]
	push rax
.L5:
	mov rax, 1
	push rax
.L6:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L7:
	lea rax, QWORD [rbp - 8]
	push rax
.L8:
	mov rax, 1
	push rax
.L9:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	jmp .L1
.L2:
.L10:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	jmp .PE.strlen
	mov rax, 0
.PE.strlen:
	leave
	ret
exit:
.PB.exit:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L11:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L12:
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
.L13:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L14:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
.L15:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rdx, rax
.L16:
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
.L17:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L18:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
.L19:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rdx, rax
.L20:
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
.L21:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L22:
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
.L23:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	push rax
.L24:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L25:
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
.L26:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rax, QWORD [rax]

	mov rdi, rax
	pop rax
	call rax
.L27:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L28:
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
.L29:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L30:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rsi, rax
.L31:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rdx, rax
.L32:
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
.L33:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L34:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rsi, rax
.L35:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rdx, rax
.L36:
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
.L37:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L38:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
.L39:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rdx, rax
.L40:
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
.L41:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L42:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
.L43:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rdx, rax
.L44:
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
.L45:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L46:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rsi, rax
.L47:
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
.L48:
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
.L49:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L50:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
.L51:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	mov rdx, rax
.L52:
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
.L53:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L54:
	mov rax, 3
	syscall
	jmp .PE.close
	mov rax, 0
.PE.close:
	leave
	ret
stoi:
.PB.stoi:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
.L55:
	mov rax, 0
	pop rdi
	mov [rdi], rax
.L56:
.L58:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	cmp rax, 0
	je .L57
.L59:
	lea rax, QWORD [rbp - 16]
	push rax
.L60:
	mov rax, 48
	push rax
.L61:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L62:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	push rax
.L63:
	mov rax, 10
	pop rbx
	mul rbx
	push rax
	pop rax
	pop rbx
	add rax, rbx
	push rax
	pop rax
	pop rbx
	sub rax, rbx
	pop rdi
	mov [rdi], rax
.L64:
	lea rax, QWORD [rbp - 8]
	push rax
.L65:
	mov rax, 1
	push rax
.L66:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	jmp .L56
.L57:
.L67:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	jmp .PE.stoi
	mov rax, 0
.PE.stoi:
	leave
	ret
itos:
.PB.itos:
	push rbp
	mov rbp, rsp
	sub rsp, 49
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
.L68:
	mov rax, 32
	pop rdi
	mov [rdi], rax
	lea rax, QWORD [rbp - 24]
	push rax
	mov rax, sbrk
	push rax
.L69:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	lea rax, QWORD [rbp - 32]
	push rax
.L70:
	mov rax, 1
	pop rdi
	mov [rdi], rax
	lea rax, BYTE [rbp - 33]
	push rax
.L71:
	mov rax, 0
	pop rdi
	mov [rdi], rax
.L73:
	mov rax, 0
	push rax
.L74:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovl rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L72
.L75:
	lea rax, QWORD [rbp - 8]
	push rax
.L76:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L77:
	mov rax, 1
	neg rax
	pop rbx
	mul rbx
	pop rdi
	mov [rdi], rax
.L78:
	lea rax, BYTE [rbp - 33]
	push rax
.L79:
	mov rax, 1
	pop rdi
	mov [rdi], rax
.L72:
.L80:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	push rax
.L81:
	mov rax, 1
	push rax
.L82:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	push rax
.L83:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	sub rax, rbx
	push rax
	pop rax
	pop rbx
	sub rax, rbx
	mov rdx, 8
	mul rdx
	pop rbx
	add rax, rbx
	push rax
.L84:
	mov rax, 48
	push rax
.L85:
	mov rax, 10
	push rax
.L86:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	pop rbx
	xor rdx, rdx
	div rbx
	mov rax, rdx
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L87:
	lea rax, QWORD [rbp - 8]
	push rax
.L88:
	mov rax, 10
	push rax
.L89:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	pop rbx
	xor rdx, rdx
	div rbx
	pop rdi
	mov [rdi], rax
.L90:
	lea rax, QWORD [rbp - 32]
	push rax
.L91:
	mov rax, 1
	push rax
.L92:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L93:
.L95:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	cmp rax, 0
	je .L94
.L96:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	push rax
.L97:
	mov rax, 1
	push rax
.L98:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	push rax
.L99:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	sub rax, rbx
	push rax
	pop rax
	pop rbx
	sub rax, rbx
	mov rdx, 8
	mul rdx
	pop rbx
	add rax, rbx
	push rax
.L100:
	mov rax, 48
	push rax
.L101:
	mov rax, 10
	push rax
.L102:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	pop rbx
	xor rdx, rdx
	div rbx
	mov rax, rdx
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L103:
	lea rax, QWORD [rbp - 8]
	push rax
.L104:
	mov rax, 10
	push rax
.L105:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	pop rbx
	xor rdx, rdx
	div rbx
	pop rdi
	mov [rdi], rax
.L106:
	lea rax, QWORD [rbp - 32]
	push rax
.L107:
	mov rax, 1
	push rax
.L108:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	jmp .L93
.L94:
.L110:
	lea rax, BYTE [rbp - 33]
	movsx rax, BYTE [rax]

	cmp rax, 0
	je .L109
.L111:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	push rax
.L112:
	mov rax, 1
	push rax
.L113:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	push rax
.L114:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	sub rax, rbx
	push rax
	pop rax
	pop rbx
	sub rax, rbx
	mov rdx, 8
	mul rdx
	pop rbx
	add rax, rbx
	push rax
.L115:
	mov rax, 45
	pop rdi
	mov [rdi], rax
.L116:
	lea rax, QWORD [rbp - 32]
	push rax
.L117:
	mov rax, 1
	push rax
.L118:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L109:
	lea rax, QWORD [rbp - 41]
	push rax
.L119:
	mov rax, 0
	pop rdi
	mov [rdi], rax
	lea rax, QWORD [rbp - 49]
	push rax
	mov rax, sbrk
	push rax
.L120:
	mov rax, 1
	push rax
.L121:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	add rax, rbx
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L122:
.L124:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	push rax
.L125:
	lea rax, QWORD [rbp - 41]
	mov rax, QWORD [rax]

	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovl rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L123
.L126:
	lea rax, QWORD [rbp - 49]
	mov rax, [rax]
	push rax
.L127:
	lea rax, QWORD [rbp - 41]
	mov rax, QWORD [rax]

	pop rbx
	add rax, rbx
	push rax
.L128:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	push rax
.L129:
	lea rax, QWORD [rbp - 41]
	mov rax, QWORD [rax]

	push rax
.L130:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	push rax
.L131:
	mov rax, 32
	push rax
	pop rax
	pop rbx
	sub rax, rbx
	push rax
	pop rax
	pop rbx
	add rax, rbx
	mov rdx, 8
	mul rdx
	pop rbx
	add rax, rbx
	mov rax, QWORD [rax]

	pop rdi
	mov [rdi], rax
.L132:
	lea rax, QWORD [rbp - 41]
	push rax
.L133:
	mov rax, 1
	push rax
.L134:
	lea rax, QWORD [rbp - 41]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	jmp .L122
.L123:
.L135:
	lea rax, QWORD [rbp - 49]
	mov rax, [rax]
	jmp .PE.itos
	mov rax, 0
.PE.itos:
	leave
	ret
fputs:
.PB.fputs:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	lea rax, QWORD [rbp - 24]
	push rax
	mov rax, strlen
	push rax
.L136:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	mov rax, write
	push rax
.L137:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L138:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
.L139:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rdx, rax
	pop rax
	call rax
	mov rax, 0
.PE.fputs:
	leave
	ret
fputi:
.PB.fputi:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	lea rax, QWORD [rbp - 24]
	push rax
	mov rax, itos
	push rax
.L140:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	mov rax, fputs
	push rax
.L141:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L142:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	mov rsi, rax
	pop rax
	call rax
	mov rax, 0
.PE.fputi:
	leave
	ret
eputs:
.PB.eputs:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
	mov rax, fputs
	push rax
.L143:
	mov rax, 2
	mov rdi, rax
.L144:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rsi, rax
	pop rax
	call rax
	mov rax, 0
.PE.eputs:
	leave
	ret
puts:
.PB.puts:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
	mov rax, fputs
	push rax
.L145:
	mov rax, 1
	mov rdi, rax
.L146:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rsi, rax
	pop rax
	call rax
	mov rax, 0
.PE.puts:
	leave
	ret
puti:
.PB.puti:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
	mov rax, fputi
	push rax
.L147:
	mov rax, 1
	mov rdi, rax
.L148:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rsi, rax
	pop rax
	call rax
	mov rax, 0
.PE.puti:
	leave
	ret
putc:
.PB.putc:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
	mov rax, sbrk
	push rax
.L149:
	mov rax, 2
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L150:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	push rax
.L151:
	mov rax, 0
	pop rbx
	add rax, rbx
	push rax
.L152:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	pop rdi
	mov [rdi], rax
.L153:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	push rax
.L154:
	mov rax, 1
	pop rbx
	add rax, rbx
	push rax
.L155:
	mov rax, 0
	pop rdi
	mov [rdi], rax
	mov rax, puts
	push rax
.L156:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	mov rax, 0
.PE.putc:
	leave
	ret
input:
.PB.input:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	mov [rbp - 8], rdi
	mov rax, fputs
	push rax
.L157:
	mov rax, 0
	mov rdi, rax
.L158:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rsi, rax
	pop rax
	call rax
	lea rax, QWORD [rbp - 16]
	push rax
.L159:
	mov rax, 256
	pop rdi
	mov [rdi], rax
	lea rax, QWORD [rbp - 24]
	push rax
	mov rax, sbrk
	push rax
.L160:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	lea rax, QWORD [rbp - 32]
	push rax
	mov rax, read
	push rax
.L161:
	mov rax, 0
	mov rdi, rax
.L162:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	mov rsi, rax
.L163:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rdx, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L164:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	push rax
.L165:
	mov rax, 1
	push rax
.L166:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	sub rax, rbx
	pop rbx
	add rax, rbx
	push rax
.L167:
	mov rax, 0
	pop rdi
	mov [rdi], rax
.L168:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	jmp .PE.input
	mov rax, 0
.PE.input:
	leave
	ret
streq:
.PB.streq:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	lea rax, QWORD [rbp - 24]
	push rax
	mov rax, strlen
	push rax
.L169:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L171:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	push rax
	mov rax, strlen
	push rax
.L172:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	mov rcx, 0
	mov rdx, 1
	cmove rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L170
	lea rax, Gfalse
	mov rax, QWORD [rax]

	jmp .PE.streq
.L170:
	lea rax, QWORD [rbp - 32]
	push rax
.L173:
	mov rax, 0
	pop rdi
	mov [rdi], rax
.L174:
.L176:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	push rax
.L177:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovl rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L175
.L179:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L180:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	pop rbx
	add rax, rbx
	movsx rax, BYTE [rax]

	push rax
.L181:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	push rax
.L182:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	pop rbx
	add rax, rbx
	movsx rax, BYTE [rax]

	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	mov rcx, 0
	mov rdx, 1
	cmove rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L178
	lea rax, Gfalse
	mov rax, QWORD [rax]

	jmp .PE.streq
.L178:
.L183:
	lea rax, QWORD [rbp - 32]
	push rax
.L184:
	mov rax, 1
	push rax
.L185:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	jmp .L174
.L175:
	lea rax, Gtrue
	mov rax, QWORD [rax]

	jmp .PE.streq
	mov rax, 0
.PE.streq:
	leave
	ret
memcpy:
.PB.memcpy:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	mov [rbp - 24], rdx
	lea rax, QWORD [rbp - 32]
	push rax
.L186:
	mov rax, 0
	pop rdi
	mov [rdi], rax
.L187:
.L189:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	push rax
.L190:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovl rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L188
.L191:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L192:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	pop rbx
	add rax, rbx
	push rax
.L193:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	push rax
.L194:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	pop rbx
	add rax, rbx
	movsx rax, BYTE [rax]

	pop rdi
	mov [rdi], rax
.L195:
	lea rax, QWORD [rbp - 32]
	push rax
.L196:
	mov rax, 1
	push rax
.L197:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	jmp .L187
.L188:
.L198:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L199:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	pop rbx
	add rax, rbx
	push rax
.L200:
	mov rax, 0
	pop rdi
	mov [rdi], rax
	mov rax, 0
.PE.memcpy:
	leave
	ret
Array_impl_begin:
.PB.Array_impl_begin:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L201:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	jmp .PE.Array_impl_begin
	mov rax, 0
.PE.Array_impl_begin:
	leave
	ret
Array_impl_end:
.PB.Array_impl_end:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L202:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	push rax
.L203:
	mov rax, 8
	pop rbx
	mul rbx
	push rax
.L204:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	jmp .PE.Array_impl_end
	mov rax, 0
.PE.Array_impl_end:
	leave
	ret
Array_impl_init:
.PB.Array_impl_init:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L205:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
	mov rax, sbrk
	push rax
.L206:
	mov rax, 8
	push rax
.L207:
	mov rax, 100
	pop rbx
	mul rbx
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L208:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	push rax
.L209:
	mov rax, 0
	pop rdi
	mov [rdi], rax
	mov rax, 0
.PE.Array_impl_init:
	leave
	ret
Array_impl_push:
.PB.Array_impl_push:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
.L210:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
.L211:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	mov rdx, 8
	mul rdx
	pop rbx
	add rax, rbx
	push rax
.L212:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	pop rdi
	mov [rdi], rax
.L213:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	push rax
.L214:
	mov rax, 1
	push rax
.L215:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	mov rax, 0
.PE.Array_impl_push:
	leave
	ret
Array_impl_back:
.PB.Array_impl_back:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L216:
	mov rax, 1
	push rax
.L217:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	sub rax, rbx
	push rax
.L218:
	mov rax, 8
	pop rbx
	mul rbx
	push rax
.L219:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	jmp .PE.Array_impl_back
	mov rax, 0
.PE.Array_impl_back:
	leave
	ret
Array_impl_len:
.PB.Array_impl_len:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L220:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	jmp .PE.Array_impl_len
	mov rax, 0
.PE.Array_impl_len:
	leave
	ret
Array_new:
.PB.Array_new:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	lea rax, QWORD [rbp - 8]
	push rax
	mov rax, sbrk
	push rax
.L221:
	mov rax, 16
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L222:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Array_impl_init
	push rax
.L223:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
.L224:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	jmp .PE.Array_new
	mov rax, 0
.PE.Array_new:
	leave
	ret
Arena_impl_init:
.PB.Arena_impl_init:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L225:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 16
	mov rax, rdi
	mov rax, Array_impl_init
	push rax
.L226:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 16
	mov rdi, rax
	pop rax
	call rax
.L227:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	push rax
.L228:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L229:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 16
	mov rax, rdi
	mov rax, Array_impl_begin
	push rax
.L230:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 16
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	pop rdi
	mov [rdi], rax
	mov rax, 0
.PE.Arena_impl_init:
	leave
	ret
Arena_impl_grow:
.PB.Arena_impl_grow:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
.L231:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 32
	push rax
	mov rax, sbrk
	push rax
.L232:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L233:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	push rax
.L234:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 32
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L235:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L236:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	push rax
.L237:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 32
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	mov rax, 0
.PE.Arena_impl_grow:
	leave
	ret
Arena_impl_alloc:
.PB.Arena_impl_alloc:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
.L239:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	cmp rax, 0
	mov rcx, 0
	mov rdx, 1
	cmove rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L238
.L240:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Arena_impl_init
	push rax
.L241:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
.L238:
.L243:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	push rax
.L244:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	push rax
.L245:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	sub rax, rbx
	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovl rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L242
.L246:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Arena_impl_grow
	push rax
.L247:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L248:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rsi, rax
	pop rax
	call rax
.L242:
	lea rax, QWORD [rbp - 24]
	push rax
.L249:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 32
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L250:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 32
	push rax
.L251:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	push rax
.L252:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 32
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L253:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	jmp .PE.Arena_impl_alloc
	mov rax, 0
.PE.Arena_impl_alloc:
	leave
	ret
Arena_impl_free:
.PB.Arena_impl_free:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
.L254:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L255:
.L257:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
.L258:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovl rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L256
.L259:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	push rax
.L260:
	mov rax, 0
	pop rdi
	mov [rdi], rax
.L261:
	lea rax, QWORD [rbp - 16]
	push rax
.L262:
	mov rax, 8
	push rax
.L263:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	jmp .L255
.L256:
	mov rax, 0
.PE.Arena_impl_free:
	leave
	ret
InternString_impl_init:
.PB.InternString_impl_init:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
.L264:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
	mov rax, sbrk
	push rax
.L265:
	mov rax, 16
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L266:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Array_impl_init
	push rax
.L267:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
.L268:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	push rax
.L269:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L270:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	mov rax, rdi
	mov rax, Arena_impl_init
	push rax
.L271:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	mov rax, 0
.PE.InternString_impl_init:
	leave
	ret
InternString_impl_intern:
.PB.InternString_impl_intern:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	lea rax, QWORD [rbp - 24]
	push rax
.L272:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Array_impl_begin
	push rax
.L273:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L274:
.L276:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Array_impl_end
	push rax
.L277:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	push rax
.L278:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovl rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L275
	lea rax, QWORD [rbp - 32]
	push rax
.L279:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
	mov rax, puts
	push rax
.L280:
	mov rax, S0
	mov rdi, rax
	pop rax
	call rax
	mov rax, puts
	push rax
.L281:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	mov rax, puts
	push rax
.L282:
	mov rax, S1
	mov rdi, rax
	pop rax
	call rax
	mov rax, streq
	push rax
.L284:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdi, rax
.L285:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L283
.L286:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	jmp .PE.InternString_impl_intern
.L283:
.L287:
	lea rax, QWORD [rbp - 24]
	push rax
.L288:
	mov rax, 8
	push rax
.L289:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	jmp .L274
.L275:
	lea rax, QWORD [rbp - 40]
	push rax
	mov rax, strlen
	push rax
.L290:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	lea rax, QWORD [rbp - 48]
	push rax
.L291:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	mov rax, rdi
	mov rax, Arena_impl_alloc
	push rax
.L292:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	mov rdi, rax
.L293:
	mov rax, 1
	push rax
.L294:
	lea rax, QWORD [rbp - 40]
	mov rax, QWORD [rax]

	push rax
	pop rax
	pop rbx
	add rax, rbx
	mov rsi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	mov rax, memcpy
	push rax
.L295:
	lea rax, QWORD [rbp - 48]
	mov rax, [rax]
	mov rdi, rax
.L296:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
.L297:
	lea rax, QWORD [rbp - 40]
	mov rax, QWORD [rax]

	mov rdx, rax
	pop rax
	call rax
.L298:
	lea rax, QWORD [rbp - 48]
	mov rax, [rax]
	push rax
.L299:
	lea rax, QWORD [rbp - 40]
	mov rax, QWORD [rax]

	mov rdx, 8
	mul rdx
	pop rbx
	add rax, rbx
	push rax
.L300:
	mov rax, 0
	pop rdi
	mov [rdi], rax
.L301:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Array_impl_push
	push rax
.L302:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	mov rdi, rax
.L303:
	lea rax, QWORD [rbp - 48]
	mov rax, [rax]
	mov rsi, rax
	pop rax
	call rax
.L304:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	jmp .PE.InternString_impl_intern
	mov rax, 0
.PE.InternString_impl_intern:
	leave
	ret
FILE_impl_isOpen:
.PB.FILE_impl_isOpen:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L305:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
.L306:
	mov rax, 0
	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovl rcx, rdx
	mov rax, rcx
	jmp .PE.FILE_impl_isOpen
	mov rax, 0
.PE.FILE_impl_isOpen:
	leave
	ret
FILE_impl_close:
.PB.FILE_impl_close:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
	mov rax, close
	push rax
.L307:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
.L308:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 16
	push rax
.L309:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 24
	push rax
.L310:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 32
	push rax
.L311:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L312:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	push rax
.L313:
	mov rax, 0
	pop rdi
	mov [rdi], rax
	pop rdi
	mov [rdi], rax
	pop rdi
	mov [rdi], rax
	pop rdi
	mov [rdi], rax
	pop rdi
	mov [rdi], rax
	mov rax, 0
.PE.FILE_impl_close:
	leave
	ret
fnew:
.PB.fnew:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	lea rax, QWORD [rbp - 8]
	push rax
	lea rax, Gstd_arena
	mov rax, rdi
	mov rax, Arena_impl_alloc
	push rax
	lea rax, Gstd_arena
	mov rdi, rax
.L314:
	mov rax, 40
	mov rsi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L315:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 16
	push rax
	lea rax, Gstd_arena
	mov rax, rdi
	mov rax, Arena_impl_alloc
	push rax
	lea rax, Gstd_arena
	mov rdi, rax
.L316:
	mov rax, 500
	mov rsi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L317:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	jmp .PE.fnew
	mov rax, 0
.PE.fnew:
	leave
	ret
fopen:
.PB.fopen:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	lea rax, QWORD [rbp - 24]
	push rax
.L318:
	mov rax, 1
	neg rax
	pop rdi
	mov [rdi], rax
	mov rax, streq
	push rax
.L320:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rdi, rax
.L321:
	mov rax, S2
	mov rsi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L319
.L322:
	lea rax, QWORD [rbp - 24]
	push rax
.L323:
	mov rax, 0
	pop rdi
	mov [rdi], rax
.L319:
	mov rax, streq
	push rax
.L326:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rdi, rax
.L327:
	mov rax, S3
	mov rsi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L325
.L328:
	lea rax, QWORD [rbp - 24]
	push rax
.L329:
	mov rax, 1
	pop rdi
	mov [rdi], rax
.L325:
	mov rax, streq
	push rax
.L331:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rdi, rax
.L332:
	mov rax, S4
	mov rsi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L330
.L333:
	lea rax, QWORD [rbp - 24]
	push rax
.L334:
	mov rax, 0
	push rax
.L335:
	mov rax, 64
	push rax
.L336:
	mov rax, 1
	pop rbx
	or rax, rbx
	pop rbx
	or rax, rbx
	pop rdi
	mov [rdi], rax
.L330:
.L324:
	lea rax, QWORD [rbp - 32]
	push rax
	lea rax, Gstd_arena
	mov rax, rdi
	mov rax, Arena_impl_alloc
	push rax
	lea rax, Gstd_arena
	mov rdi, rax
.L337:
	mov rax, 40
	mov rsi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	lea rax, QWORD [rbp - 40]
	push rax
.L338:
	mov rax, 128
	push rax
.L339:
	mov rax, 2
	push rax
.L340:
	mov rax, 256
	push rax
.L341:
	mov rax, 4
	pop rbx
	or rax, rbx
	pop rbx
	or rax, rbx
	pop rbx
	or rax, rbx
	pop rdi
	mov [rdi], rax
.L342:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	push rax
	mov rax, open
	push rax
.L343:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L344:
	lea rax, QWORD [rbp - 24]
	mov rax, QWORD [rax]

	mov rsi, rax
.L345:
	lea rax, QWORD [rbp - 40]
	mov rax, QWORD [rax]

	mov rdx, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L346:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	add rax, 32
	push rax
	lea rax, Gstd_arena
	mov rax, rdi
	mov rax, Arena_impl_alloc
	push rax
	lea rax, Gstd_arena
	mov rdi, rax
.L347:
	mov rax, 4096
	mov rsi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	lea rax, QWORD [rbp - 48]
	push rax
	mov rax, read
	push rax
.L348:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rax, [rax]
	mov rdi, rax
.L349:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	add rax, 32
	mov rax, [rax]
	mov rsi, rax
.L350:
	mov rax, 4096
	mov rdx, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L351:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	add rax, 16
	push rax
.L352:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	add rax, 32
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L353:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	add rax, 24
	push rax
.L354:
	lea rax, QWORD [rbp - 48]
	mov rax, QWORD [rax]

	push rax
.L355:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	add rax, 32
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L356:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	jmp .PE.fopen
	mov rax, 0
.PE.fopen:
	leave
	ret
LexerUtil_impl_isDigit:
.PB.LexerUtil_impl_isDigit:
	push rbp
	mov rbp, rsp
	sub rsp, 9
	mov [rbp - 8], rdi
	mov [rbp - 9], rsi
.L360:
	lea rax, BYTE [rbp - 9]
	movsx rax, BYTE [rax]

	push rax
.L361:
	mov rax, 48
	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovle rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L357
.L362:
	mov rax, 57
	push rax
.L363:
	lea rax, BYTE [rbp - 9]
	movsx rax, BYTE [rax]

	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovle rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L357
.L358:
	mov rax, 1
	jmp .L359
.L357:
	mov rax, 0
.L359:
	jmp .PE.LexerUtil_impl_isDigit
	mov rax, 0
.PE.LexerUtil_impl_isDigit:
	leave
	ret
LexerUtil_impl_isLiteral:
.PB.LexerUtil_impl_isLiteral:
	push rbp
	mov rbp, rsp
	sub rsp, 9
	mov [rbp - 8], rdi
	mov [rbp - 9], rsi
.L367:
	lea rax, BYTE [rbp - 9]
	movsx rax, BYTE [rax]

	push rax
.L368:
	mov rax, 97
	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovle rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L364
.L369:
	mov rax, 122
	push rax
.L370:
	lea rax, BYTE [rbp - 9]
	movsx rax, BYTE [rax]

	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovle rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L364
.L365:
	mov rax, 1
	jmp .L366
.L364:
	mov rax, 0
.L366:
	cmp rax, 0
	push rax
.L374:
	lea rax, BYTE [rbp - 9]
	movsx rax, BYTE [rax]

	push rax
.L375:
	mov rax, 65
	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovle rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L371
.L376:
	mov rax, 90
	push rax
.L377:
	lea rax, BYTE [rbp - 9]
	movsx rax, BYTE [rax]

	pop rbx
	mov rcx, 0
	mov rdx, 1
	cmp rax, rbx
	cmovle rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L371
.L372:
	mov rax, 1
	jmp .L373
.L371:
	mov rax, 0
.L373:
	cmp rax, 0
	pop rbx
	add rax, rbx
	cmp rax, 0
	push rax
.L378:
	lea rax, BYTE [rbp - 9]
	movsx rax, BYTE [rax]

	push rax
.L379:
	mov rax, 95
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	pop rbx
	add rax, rbx
	jmp .PE.LexerUtil_impl_isLiteral
	mov rax, 0
.PE.LexerUtil_impl_isLiteral:
	leave
	ret
Lexer_impl_ret_token_kind_range:
.PB.Lexer_impl_ret_token_kind_range:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	mov [rbp - 24], rdx
	lea rax, QWORD [rbp - 32]
	push rax
.L380:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	push rax
.L381:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	sub rax, rbx
	pop rdi
	mov [rdi], rax
	lea rax, Gtoken
	add rax, 8
	push rax
	lea rax, Gstd_arena
	mov rax, rdi
	mov rax, Arena_impl_alloc
	push rax
	lea rax, Gstd_arena
	mov rdi, rax
.L382:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	mov rsi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	mov rax, memcpy
	push rax
	lea rax, Gtoken
	add rax, 8
	mov rax, [rax]
	mov rdi, rax
.L383:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	mov rsi, rax
.L384:
	lea rax, QWORD [rbp - 32]
	mov rax, QWORD [rax]

	mov rdx, rax
	pop rax
	call rax
	lea rax, Gtoken
	push rax
.L385:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	pop rdi
	mov [rdi], rax
	lea rax, Gtoken
	add rax, 16
	push rax
	mov rax, stoi
	push rax
	lea rax, Gtoken
	add rax, 8
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	lea rax, Gtoken
	jmp .PE.Lexer_impl_ret_token_kind_range
	mov rax, 0
.PE.Lexer_impl_ret_token_kind_range:
	leave
	ret
Lexer_impl_next:
.PB.Lexer_impl_next:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
.L386:
	mov rax, 0
	pop rdi
	mov [rdi], rax
	lea rax, QWORD [rbp - 24]
	push rax
	lea rax, Gnull
	mov rax, QWORD [rax]

	pop rdi
	mov [rdi], rax
.L387:
.L389:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L390:
	mov rax, 32
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	push rax
.L391:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L392:
	mov rax, 10
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	pop rbx
	add rax, rbx
	cmp rax, 0
	je .L388
.L393:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L394:
	mov rax, 1
	push rax
.L395:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	jmp .L387
.L388:
	lea rax, QWORD [rbp - 32]
	push rax
.L396:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L398:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L399:
	mov rax, 0
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L397
.L400:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_ret_token_kind_range
	push rax
.L401:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L402:
	mov rax, 0
	mov rsi, rax
.L403:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	jmp .PE.Lexer_impl_next
.L397:
.L405:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, rdi
	mov rax, LexerUtil_impl_isDigit
	push rax
.L406:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rdi, rax
.L407:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	mov rsi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L404
.L408:
.L410:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, rdi
	mov rax, LexerUtil_impl_isDigit
	push rax
.L411:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rdi, rax
.L412:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	mov rsi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L409
.L413:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L414:
	mov rax, 1
	push rax
.L415:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	jmp .L408
.L409:
.L416:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_ret_token_kind_range
	push rax
.L417:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L418:
	mov rax, 34
	mov rsi, rax
.L419:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	jmp .PE.Lexer_impl_next
.L404:
.L421:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, rdi
	mov rax, LexerUtil_impl_isLiteral
	push rax
.L422:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rdi, rax
.L423:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	mov rsi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L420
.L424:
.L426:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, rdi
	mov rax, LexerUtil_impl_isLiteral
	push rax
.L427:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rdi, rax
.L428:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	mov rsi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L425
.L429:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L430:
	mov rax, 1
	push rax
.L431:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
	jmp .L424
.L425:
.L432:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_ret_token_kind_range
	push rax
.L433:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L434:
	mov rax, 32
	mov rsi, rax
.L435:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	jmp .PE.Lexer_impl_next
.L420:
.L437:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L438:
	mov rax, 43
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L436
.L439:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L440:
	mov rax, 1
	push rax
.L441:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L442:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_ret_token_kind_range
	push rax
.L443:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L444:
	mov rax, 10
	mov rsi, rax
.L445:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	jmp .PE.Lexer_impl_next
.L436:
.L447:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L448:
	mov rax, 45
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L446
.L449:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L450:
	mov rax, 1
	push rax
.L451:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L452:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_ret_token_kind_range
	push rax
.L453:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L454:
	mov rax, 12
	mov rsi, rax
.L455:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	jmp .PE.Lexer_impl_next
.L446:
.L457:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L458:
	mov rax, 42
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L456
.L459:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L460:
	mov rax, 1
	push rax
.L461:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L462:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_ret_token_kind_range
	push rax
.L463:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L464:
	mov rax, 17
	mov rsi, rax
.L465:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	jmp .PE.Lexer_impl_next
.L456:
.L467:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L468:
	mov rax, 47
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L466
.L469:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L470:
	mov rax, 1
	push rax
.L471:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L472:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_ret_token_kind_range
	push rax
.L473:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L474:
	mov rax, 22
	mov rsi, rax
.L475:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	jmp .PE.Lexer_impl_next
.L466:
.L477:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L478:
	mov rax, 59
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L476
.L479:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L480:
	mov rax, 1
	push rax
.L481:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L482:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_ret_token_kind_range
	push rax
.L483:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L484:
	mov rax, 3
	mov rsi, rax
.L485:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	jmp .PE.Lexer_impl_next
.L476:
.L487:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L488:
	mov rax, 44
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L486
.L489:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L490:
	mov rax, 1
	push rax
.L491:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L492:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_ret_token_kind_range
	push rax
.L493:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L494:
	mov rax, 27
	mov rsi, rax
.L495:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	jmp .PE.Lexer_impl_next
.L486:
.L497:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L498:
	mov rax, 46
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L496
.L499:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L500:
	mov rax, 1
	push rax
.L501:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L502:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_ret_token_kind_range
	push rax
.L503:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L504:
	mov rax, 2
	mov rsi, rax
.L505:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	jmp .PE.Lexer_impl_next
.L496:
.L507:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L508:
	mov rax, 40
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L506
.L509:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L510:
	mov rax, 1
	push rax
.L511:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L512:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_ret_token_kind_range
	push rax
.L513:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L514:
	mov rax, 20
	mov rsi, rax
.L515:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	jmp .PE.Lexer_impl_next
.L506:
.L517:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L518:
	mov rax, 41
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L516
.L519:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L520:
	mov rax, 1
	push rax
.L521:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L522:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_ret_token_kind_range
	push rax
.L523:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L524:
	mov rax, 21
	mov rsi, rax
.L525:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	jmp .PE.Lexer_impl_next
.L516:
.L527:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	movsx rax, BYTE [rax]

	push rax
.L528:
	mov rax, 58
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L526
.L529:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L530:
	mov rax, 1
	push rax
.L531:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
	pop rax
	pop rbx
	add rax, rbx
	pop rdi
	mov [rdi], rax
.L532:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_ret_token_kind_range
	push rax
.L533:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
.L534:
	mov rax, 28
	mov rsi, rax
.L535:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	jmp .PE.Lexer_impl_next
.L526:
	mov rax, puts
	push rax
.L536:
	mov rax, S5
	mov rdi, rax
	pop rax
	call rax
	mov rax, exit
	push rax
.L537:
	mov rax, 1
	mov rdi, rax
	pop rax
	call rax
	mov rax, 0
.PE.Lexer_impl_next:
	leave
	ret
Lexer_impl_init:
.PB.Lexer_impl_init:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
.L538:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	push rax
.L539:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L540:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, rdi
	mov rax, Lexer_impl_next
	push rax
.L541:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	mov rax, 0
.PE.Lexer_impl_init:
	leave
	ret
make_expr_init:
.PB.make_expr_init:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
	lea rax, Gstd_arena
	mov rax, rdi
	mov rax, Arena_impl_alloc
	push rax
	lea rax, Gstd_arena
	mov rdi, rax
.L542:
	mov rax, 32
	mov rsi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L543:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	push rax
.L544:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	pop rdi
	mov [rdi], rax
.L545:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	jmp .PE.make_expr_init
	mov rax, 0
.PE.make_expr_init:
	leave
	ret
make_expr_int:
.PB.make_expr_int:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
	mov rax, make_expr_init
	push rax
.L546:
	mov rax, 1
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L547:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	add rax, 8
	push rax
.L548:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	pop rdi
	mov [rdi], rax
.L549:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	jmp .PE.make_expr_int
	mov rax, 0
.PE.make_expr_int:
	leave
	ret
make_expr_name:
.PB.make_expr_name:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
	mov rax, make_expr_init
	push rax
.L550:
	mov rax, 4
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L551:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	add rax, 8
	push rax
.L552:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L553:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	jmp .PE.make_expr_name
	mov rax, 0
.PE.make_expr_name:
	leave
	ret
make_expr_unary:
.PB.make_expr_unary:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	lea rax, QWORD [rbp - 24]
	push rax
	mov rax, make_expr_init
	push rax
.L554:
	mov rax, 11
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L555:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	add rax, 8
	push rax
.L556:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	pop rdi
	mov [rdi], rax
.L557:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	add rax, 8
	add rax, 8
	push rax
.L558:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L559:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	jmp .PE.make_expr_unary
	mov rax, 0
.PE.make_expr_unary:
	leave
	ret
make_expr_binary:
.PB.make_expr_binary:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	mov [rbp - 24], rdx
	lea rax, QWORD [rbp - 32]
	push rax
	mov rax, make_expr_init
	push rax
.L560:
	mov rax, 12
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L561:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	add rax, 8
	push rax
.L562:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	pop rdi
	mov [rdi], rax
.L563:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	add rax, 8
	add rax, 8
	push rax
.L564:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L565:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	add rax, 8
	add rax, 16
	push rax
.L566:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L567:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	jmp .PE.make_expr_binary
	mov rax, 0
.PE.make_expr_binary:
	leave
	ret
print_expr:
.PB.print_expr:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L569:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	cmp rax, 0
	mov rcx, 0
	mov rdx, 1
	cmove rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L568
	lea rax, Gnull
	mov rax, QWORD [rax]

	jmp .PE.print_expr
.L568:
.L571:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
.L572:
	mov rax, 1
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L570
	mov rax, puts
	push rax
.L573:
	mov rax, S6
	mov rdi, rax
	pop rax
	call rax
	mov rax, puti
	push rax
.L574:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	mov rax, puts
	push rax
.L575:
	mov rax, S7
	mov rdi, rax
	pop rax
	call rax
	lea rax, Gnull
	mov rax, QWORD [rax]

	jmp .PE.print_expr
.L570:
.L577:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
.L578:
	mov rax, 4
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L576
	mov rax, puts
	push rax
.L579:
	mov rax, S8
	mov rdi, rax
	pop rax
	call rax
	mov rax, puts
	push rax
.L580:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	mov rax, puts
	push rax
.L581:
	mov rax, S9
	mov rdi, rax
	pop rax
	call rax
	lea rax, Gnull
	mov rax, QWORD [rax]

	jmp .PE.print_expr
.L576:
.L583:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
.L584:
	mov rax, 12
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L582
	mov rax, puts
	push rax
.L585:
	mov rax, S10
	mov rdi, rax
	pop rax
	call rax
	mov rax, print_expr
	push rax
.L586:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	add rax, 8
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	mov rax, puts
	push rax
.L587:
	mov rax, S11
	mov rdi, rax
	pop rax
	call rax
	mov rax, print_expr
	push rax
.L588:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	add rax, 16
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	mov rax, puts
	push rax
.L589:
	mov rax, S12
	mov rdi, rax
	pop rax
	call rax
	lea rax, Gnull
	mov rax, QWORD [rax]

	jmp .PE.print_expr
.L582:
.L591:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	push rax
.L592:
	mov rax, 11
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L590
	mov rax, puts
	push rax
.L593:
	mov rax, S13
	mov rdi, rax
	pop rax
	call rax
.L594:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	mov rdi, rax
	call dump
	mov rax, puts
	push rax
.L595:
	mov rax, S14
	mov rdi, rax
	pop rax
	call rax
	mov rax, print_expr
	push rax
.L596:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	add rax, 8
	add rax, 8
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	mov rax, puts
	push rax
.L597:
	mov rax, S15
	mov rdi, rax
	pop rax
	call rax
	lea rax, Gnull
	mov rax, QWORD [rax]

	jmp .PE.print_expr
.L590:
	mov rax, puts
	push rax
.L598:
	mov rax, S16
	mov rdi, rax
	pop rax
	call rax
	mov rax, puti
	push rax
.L599:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	mov rax, puts
	push rax
.L600:
	mov rax, S17
	mov rdi, rax
	pop rax
	call rax
	mov rax, 0
.PE.print_expr:
	leave
	ret
__stmt_init:
.PB.__stmt_init:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
	lea rax, Gstd_arena
	mov rax, rdi
	mov rax, Arena_impl_alloc
	push rax
	lea rax, Gstd_arena
	mov rdi, rax
.L601:
	mov rax, 16
	mov rsi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L602:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	push rax
.L603:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	pop rdi
	mov [rdi], rax
.L604:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	jmp .PE.__stmt_init
	mov rax, 0
.PE.__stmt_init:
	leave
	ret
make_stmt_expr:
.PB.make_stmt_expr:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	lea rax, QWORD [rbp - 16]
	push rax
	mov rax, __stmt_init
	push rax
.L605:
	mov rax, 1
	mov rdi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L606:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	add rax, 8
	push rax
.L607:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
.L608:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	jmp .PE.make_stmt_expr
	mov rax, 0
.PE.make_stmt_expr:
	leave
	ret
expr_literal_assign:
.PB.expr_literal_assign:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
	lea rax, Gtoken
	mov rax, [rax]
	push rax
.L610:
	mov rax, 28
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	mov rcx, 0
	mov rdx, 1
	cmove rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L609
	mov rax, puts
	push rax
.L611:
	mov rax, S18
	mov rdi, rax
	pop rax
	call rax
	mov rax, exit
	push rax
.L612:
	mov rax, 1
	mov rdi, rax
	pop rax
	call rax
.L609:
	lea rax, Glexer
	mov rax, rdi
	mov rax, Lexer_impl_next
	push rax
	lea rax, Glexer
	mov rdi, rax
	pop rax
	call rax
	mov rax, 0
.PE.expr_literal_assign:
	leave
	ret
expr_literal:
.PB.expr_literal:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	lea rax, Gtoken
	mov rax, [rax]
	push rax
.L614:
	mov rax, 34
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L613
.L615:
	lea rax, QWORD [rbp - 8]
	push rax
	lea rax, Gtoken
	add rax, 16
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
	lea rax, Glexer
	mov rax, rdi
	mov rax, Lexer_impl_next
	push rax
	lea rax, Glexer
	mov rdi, rax
	pop rax
	call rax
	mov rax, make_expr_int
	push rax
.L616:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
	pop rax
	call rax
	jmp .PE.expr_literal
.L613:
	lea rax, Gtoken
	mov rax, [rax]
	push rax
.L618:
	mov rax, 32
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L617
.L619:
	lea rax, QWORD [rbp - 8]
	push rax
	lea rax, Gtoken
	add rax, 8
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
	lea rax, Glexer
	mov rax, rdi
	mov rax, Lexer_impl_next
	push rax
	lea rax, Glexer
	mov rdi, rax
	pop rax
	call rax
	lea rax, Gtoken
	mov rax, [rax]
	push rax
.L621:
	mov rax, 28
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L620
	mov rax, expr_literal_assign
	push rax
.L622:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
	pop rax
	call rax
	jmp .PE.expr_literal
.L620:
	mov rax, make_expr_name
	push rax
.L623:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
	pop rax
	call rax
	jmp .PE.expr_literal
.L617:
	mov rax, puts
	push rax
.L624:
	mov rax, S19
	mov rdi, rax
	pop rax
	call rax
	mov rax, exit
	push rax
.L625:
	mov rax, 1
	mov rdi, rax
	pop rax
	call rax
	mov rax, 0
.PE.expr_literal:
	leave
	ret
isBase:
.PB.isBase:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L626:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L627:
	mov rax, 29
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	push rax
.L628:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L629:
	mov rax, 2
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	pop rbx
	add rax, rbx
	cmp rax, 0
	push rax
.L630:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L631:
	mov rax, 20
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	pop rbx
	add rax, rbx
	cmp rax, 0
	push rax
.L632:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L633:
	mov rax, 23
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	pop rbx
	add rax, rbx
	jmp .PE.isBase
	mov rax, 0
.PE.isBase:
	leave
	ret
expr_base:
.PB.expr_base:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	lea rax, QWORD [rbp - 8]
	push rax
	mov rax, expr_literal
	call rax
	pop rdi
	mov [rdi], rax
.L634:
	mov rax, isBase
	push rax
	lea rax, Gtoken
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L635
	mov rax, puts
	push rax
.L636:
	mov rax, S20
	mov rdi, rax
	pop rax
	call rax
	mov rax, exit
	push rax
.L637:
	mov rax, 1
	mov rdi, rax
	pop rax
	call rax
	jmp .L634
.L635:
.L638:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	jmp .PE.expr_base
	mov rax, 0
.PE.expr_base:
	leave
	ret
isUnary:
.PB.isUnary:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L639:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L640:
	mov rax, 10
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	push rax
.L641:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L642:
	mov rax, 12
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	pop rbx
	add rax, rbx
	cmp rax, 0
	push rax
.L643:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L644:
	mov rax, 14
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	pop rbx
	add rax, rbx
	cmp rax, 0
	push rax
.L645:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L646:
	mov rax, 17
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	pop rbx
	add rax, rbx
	cmp rax, 0
	push rax
.L647:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L648:
	mov rax, 11
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	pop rbx
	add rax, rbx
	jmp .PE.isUnary
	mov rax, 0
.PE.isUnary:
	leave
	ret
expr_unary:
.PB.expr_unary:
	push rbp
	mov rbp, rsp
	sub rsp, 16
.L649:
	mov rax, isUnary
	push rax
	lea rax, Gtoken
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L650
	lea rax, QWORD [rbp - 8]
	push rax
	lea rax, Gtoken
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
	lea rax, Glexer
	mov rax, rdi
	mov rax, Lexer_impl_next
	push rax
	lea rax, Glexer
	mov rdi, rax
	pop rax
	call rax
	lea rax, QWORD [rbp - 16]
	push rax
	mov rax, expr_unary
	call rax
	pop rdi
	mov [rdi], rax
	mov rax, make_expr_unary
	push rax
.L651:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L652:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
	pop rax
	call rax
	jmp .PE.expr_unary
	jmp .L649
.L650:
	mov rax, expr_base
	call rax
	jmp .PE.expr_unary
	mov rax, 0
.PE.expr_unary:
	leave
	ret
isMult:
.PB.isMult:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L653:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L654:
	mov rax, 17
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	push rax
.L655:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L656:
	mov rax, 22
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	pop rbx
	add rax, rbx
	jmp .PE.isMult
	mov rax, 0
.PE.isMult:
	leave
	ret
expr_mult:
.PB.expr_mult:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	lea rax, QWORD [rbp - 8]
	push rax
	mov rax, expr_unary
	call rax
	pop rdi
	mov [rdi], rax
.L657:
	mov rax, isMult
	push rax
	lea rax, Gtoken
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L658
	lea rax, QWORD [rbp - 16]
	push rax
	lea rax, Gtoken
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
	lea rax, Glexer
	mov rax, rdi
	mov rax, Lexer_impl_next
	push rax
	lea rax, Glexer
	mov rdi, rax
	pop rax
	call rax
	lea rax, QWORD [rbp - 24]
	push rax
	mov rax, expr_unary
	call rax
	pop rdi
	mov [rdi], rax
.L659:
	lea rax, QWORD [rbp - 8]
	push rax
	mov rax, make_expr_binary
	push rax
.L660:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rdi, rax
.L661:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rsi, rax
.L662:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	jmp .L657
.L658:
.L663:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	jmp .PE.expr_mult
	mov rax, 0
.PE.expr_mult:
	leave
	ret
isAdd:
.PB.isAdd:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov [rbp - 8], rdi
.L664:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L665:
	mov rax, 10
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	push rax
.L666:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L667:
	mov rax, 12
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	pop rbx
	add rax, rbx
	jmp .PE.isAdd
	mov rax, 0
.PE.isAdd:
	leave
	ret
expr_add:
.PB.expr_add:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	lea rax, QWORD [rbp - 8]
	push rax
	mov rax, expr_mult
	call rax
	pop rdi
	mov [rdi], rax
.L668:
	mov rax, isAdd
	push rax
	lea rax, Gtoken
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L669
	lea rax, QWORD [rbp - 16]
	push rax
	lea rax, Gtoken
	mov rax, [rax]
	pop rdi
	mov [rdi], rax
	lea rax, Glexer
	mov rax, rdi
	mov rax, Lexer_impl_next
	push rax
	lea rax, Glexer
	mov rdi, rax
	pop rax
	call rax
	lea rax, QWORD [rbp - 24]
	push rax
	mov rax, expr_mult
	call rax
	pop rdi
	mov [rdi], rax
.L670:
	lea rax, QWORD [rbp - 8]
	push rax
	mov rax, make_expr_binary
	push rax
.L671:
	lea rax, QWORD [rbp - 16]
	mov rax, QWORD [rax]

	mov rdi, rax
.L672:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	mov rsi, rax
.L673:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	mov rdx, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
	jmp .L668
.L669:
.L674:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	jmp .PE.expr_add
	mov rax, 0
.PE.expr_add:
	leave
	ret
expr_cmp:
.PB.expr_cmp:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	lea rax, QWORD [rbp - 8]
	push rax
	mov rax, expr_add
	call rax
	pop rdi
	mov [rdi], rax
.L675:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	jmp .PE.expr_cmp
	mov rax, 0
.PE.expr_cmp:
	leave
	ret
expr_logic:
.PB.expr_logic:
	push rbp
	mov rbp, rsp
	sub rsp, 24
	mov rax, streq
	push rax
	lea rax, Gtoken
	add rax, 8
	mov rax, [rax]
	mov rdi, rax
.L677:
	mov rax, S21
	mov rsi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L676
	lea rax, Glexer
	mov rax, rdi
	mov rax, Lexer_impl_next
	push rax
	lea rax, Glexer
	mov rdi, rax
	pop rax
	call rax
	lea rax, QWORD [rbp - 8]
	push rax
.L678:
	mov rax, 14
	pop rdi
	mov [rdi], rax
	lea rax, QWORD [rbp - 16]
	push rax
	mov rax, expr_logic
	call rax
	pop rdi
	mov [rdi], rax
	mov rax, make_expr_unary
	push rax
.L679:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	mov rdi, rax
.L680:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rsi, rax
	pop rax
	call rax
	jmp .PE.expr_logic
.L676:
	lea rax, QWORD [rbp - 24]
	push rax
	mov rax, expr_cmp
	call rax
	pop rdi
	mov [rdi], rax
.L681:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	jmp .PE.expr_logic
	mov rax, 0
.PE.expr_logic:
	leave
	ret
expr_cast:
.PB.expr_cast:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	lea rax, QWORD [rbp - 8]
	push rax
	mov rax, expr_logic
	call rax
	pop rdi
	mov [rdi], rax
.L682:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	jmp .PE.expr_cast
	mov rax, 0
.PE.expr_cast:
	leave
	ret
expr_assign:
.PB.expr_assign:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	lea rax, QWORD [rbp - 8]
	push rax
	mov rax, expr_cast
	call rax
	pop rdi
	mov [rdi], rax
	mov rax, streq
	push rax
.L684:
	mov rax, S22
	mov rdi, rax
	lea rax, Gtoken
	add rax, 8
	mov rax, [rax]
	mov rsi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L683
	mov rax, puts
	push rax
.L685:
	mov rax, S23
	mov rdi, rax
	pop rax
	call rax
	mov rax, puts
	push rax
	lea rax, Gtoken
	add rax, 8
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	mov rax, exit
	push rax
.L686:
	mov rax, 1
	mov rdi, rax
	pop rax
	call rax
.L683:
.L687:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	jmp .PE.expr_assign
	mov rax, 0
.PE.expr_assign:
	leave
	ret
expr_ternary:
.PB.expr_ternary:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	lea rax, QWORD [rbp - 8]
	push rax
	mov rax, expr_assign
	call rax
	pop rdi
	mov [rdi], rax
	mov rax, streq
	push rax
	lea rax, Gtoken
	add rax, 8
	mov rax, [rax]
	mov rdi, rax
.L689:
	mov rax, S24
	mov rsi, rax
	pop rax
	call rax
	cmp rax, 0
	je .L688
	mov rax, puts
	push rax
.L690:
	mov rax, S25
	mov rdi, rax
	pop rax
	call rax
	mov rax, exit
	push rax
.L691:
	mov rax, 1
	mov rdi, rax
	pop rax
	call rax
.L688:
.L692:
	lea rax, QWORD [rbp - 8]
	mov rax, [rax]
	jmp .PE.expr_ternary
	mov rax, 0
.PE.expr_ternary:
	leave
	ret
expr_parse:
.PB.expr_parse:
	push rbp
	mov rbp, rsp
	mov rax, expr_ternary
	call rax
	jmp .PE.expr_parse
	mov rax, 0
.PE.expr_parse:
	leave
	ret
compile_expr:
.PB.compile_expr:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
.L694:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	mov rax, [rax]
	push rax
.L695:
	mov rax, 1
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	je .L693
	mov rax, puts
	push rax
.L696:
	mov rax, S26
	mov rdi, rax
	pop rax
	call rax
	mov rax, puti
	push rax
.L697:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	add rax, 8
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	mov rax, puts
	push rax
.L698:
	mov rax, S27
	mov rdi, rax
	pop rax
	call rax
.L693:
	mov rax, 0
.PE.compile_expr:
	leave
	ret
main:
.PB.main:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	lea rax, Gstd_intern_strings
	mov rax, rdi
	mov rax, InternString_impl_init
	push rax
	lea rax, Gstd_intern_strings
	mov rdi, rax
	lea rax, Gstd_arena
	mov rsi, rax
	pop rax
	call rax
.L700:
	lea rax, QWORD [rbp - 8]
	mov rax, QWORD [rax]

	push rax
.L701:
	mov rax, 2
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	mov rcx, 0
	mov rdx, 1
	cmove rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L699
	mov rax, puts
	push rax
.L702:
	mov rax, S28
	mov rdi, rax
	pop rax
	call rax
	mov rax, exit
	push rax
.L703:
	mov rax, 1
	mov rdi, rax
	pop rax
	call rax
.L699:
	lea rax, QWORD [rbp - 24]
	push rax
	mov rax, fopen
	push rax
.L704:
	lea rax, QWORD [rbp - 16]
	mov rax, [rax]
	push rax
.L705:
	mov rax, 1
	mov rdx, 8
	mul rdx
	pop rbx
	add rax, rbx
	mov rax, [rax]
	mov rdi, rax
.L706:
	mov rax, S29
	mov rsi, rax
	pop rax
	call rax
	pop rdi
	mov [rdi], rax
.L708:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	mov rax, rdi
	mov rax, FILE_impl_isOpen
	push rax
.L709:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	mov rdi, rax
	pop rax
	call rax
	cmp rax, 0
	mov rcx, 0
	mov rdx, 1
	cmove rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L707
	mov rax, puts
	push rax
.L710:
	mov rax, S30
	mov rdi, rax
	pop rax
	call rax
	mov rax, exit
	push rax
.L711:
	mov rax, 1
	mov rdi, rax
	pop rax
	call rax
.L707:
	lea rax, Glexer
	mov rax, rdi
	mov rax, Lexer_impl_init
	push rax
	lea rax, Glexer
	mov rdi, rax
.L712:
	lea rax, QWORD [rbp - 24]
	mov rax, [rax]
	add rax, 32
	mov rax, [rax]
	mov rsi, rax
	pop rax
	call rax
.L713:
	lea rax, Gtoken
	mov rax, [rax]
	push rax
.L715:
	mov rax, 0
	mov rbx, rax
	pop rcx
	mov rdx, 1
	xor rax, rax
	cmp rbx, rcx
	cmove rax, rdx
	cmp rax, 0
	mov rcx, 0
	mov rdx, 1
	cmove rcx, rdx
	mov rax, rcx
	cmp rax, 0
	je .L714
	lea rax, QWORD [rbp - 32]
	push rax
	mov rax, expr_parse
	call rax
	pop rdi
	mov [rdi], rax
	mov rax, compile_expr
	push rax
.L716:
	mov rax, 1
	mov rdi, rax
.L717:
	lea rax, QWORD [rbp - 32]
	mov rax, [rax]
	mov rsi, rax
	pop rax
	call rax
	jmp .L713
.L714:
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
.L718:
	mov rax, 1
	pop rdi
	mov [rdi], rax
	lea rax, Gfalse
	push rax
.L719:
	mov rax, 0
	pop rdi
	mov [rdi], rax
	lea rax, Gnull
	push rax
.L720:
	mov rax, 0
	pop rdi
	mov [rdi], rax
	mov rdi, [rsp]
	mov rsi, rsp
	add rsi, 8

	call main
	mov rax, 60
	mov rdi, 0
	syscall
segment .data
	S0: db 73, 78, 84, 69, 82, 78, 32, 61, 32, 0
	S1: db 10, 0
	S2: db 114, 0
	S3: db 119, 0
	S4: db 114, 43, 0
	S5: db 85, 78, 75, 78, 79, 87, 78, 32, 84, 79, 75, 69, 78, 46, 10, 0
	S6: db 60, 69, 120, 112, 114, 32, 105, 110, 116, 58, 32, 0
	S7: db 62, 0
	S8: db 60, 69, 120, 112, 114, 32, 110, 97, 109, 101, 58, 32, 0
	S9: db 62, 0
	S10: db 60, 69, 120, 112, 114, 32, 66, 105, 110, 97, 114, 121, 58, 32, 0
	S11: db 32, 45, 32, 0
	S12: db 62, 10, 0
	S13: db 60, 69, 120, 112, 114, 32, 85, 110, 97, 114, 121, 58, 32, 0
	S14: db 32, 45, 32, 40, 0
	S15: db 41, 62, 0
	S16: db 73, 68, 75, 32, 72, 79, 87, 32, 84, 79, 32, 80, 82, 73, 78, 84, 32, 84, 72, 73, 83, 32, 75, 73, 78, 68, 58, 32, 0
	S17: db 33, 33, 10, 0
	S18: db 119, 116, 102, 32, 119, 104, 121, 32, 117, 32, 99, 97, 108, 108, 101, 100, 32, 109, 101, 63, 10, 0
	S19: db 91, 69, 82, 82, 79, 82, 93, 58, 32, 69, 120, 112, 101, 99, 116, 101, 100, 32, 69, 120, 112, 114, 101, 115, 115, 105, 111, 110, 46, 10, 0
	S20: db 66, 65, 83, 69, 32, 85, 78, 73, 77, 80, 76, 69, 77, 69, 78, 84, 69, 68, 10, 0
	S21: db 110, 101, 103, 0
	S22: db 97, 115, 0
	S23: db 67, 65, 83, 84, 32, 61, 32, 0
	S24: db 105, 102, 0
	S25: db 73, 70, 32, 78, 79, 84, 32, 73, 77, 80, 76, 69, 77, 69, 84, 69, 68, 32, 89, 69, 84, 46, 10, 0
	S26: db 9, 109, 111, 118, 32, 114, 97, 120, 44, 32, 0
	S27: db 10, 0
	S28: db 91, 69, 82, 82, 79, 82, 93, 58, 32, 101, 120, 112, 101, 99, 116, 101, 100, 32, 112, 114, 111, 103, 114, 97, 109, 32, 60, 105, 110, 112, 117, 116, 45, 102, 105, 108, 101, 112, 97, 116, 104, 62, 10, 0
	S29: db 114, 0
	S30: db 67, 111, 117, 108, 100, 32, 110, 111, 116, 32, 111, 112, 101, 110, 32, 109, 97, 107, 101, 102, 105, 108, 101, 46, 10, 0
segment .bss
	__heap_end__: resq 1
	__va_offset__: resq 1
	Gtrue: resb 8
	Gfalse: resb 8
	Gnull: resb 8
	Gstd_arena: resb 40
	Gstd_intern_strings: resb 16
	Gtoken: resb 24
	Glexer: resb 8
