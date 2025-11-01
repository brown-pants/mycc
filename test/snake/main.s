	.section .rodata
.str0:
	.string "TERM=xterm clear"
	.global new_food
	.text
new_food:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	call rand
	movq %rax, %rax
	movq %rax, %rbx
	addq $1, %rbx
	movq Width(%rip), %rax
	subq $1, %rax
	movq %rax, %rdi
	pushq %rax
	movq %rbx, %rax
	cqto
	idivq %rdi
	movq %rdx, %rcx
	popq %rax
	movq %rcx, -8(%rbp)
	call rand
	movq %rax, %rcx
	movq %rcx, %rax
	addq $1, %rax
	movq Height(%rip), %rcx
	subq $1, %rcx
	pushq %rax
	movq %rax, %rax
	cqto
	idivq %rcx
	movq %rdx, %rbx
	popq %rax
	movq %rbx, -16(%rbp)
	leaq Map(%rip), %rbx
	movq -16(%rbp), %rcx
	imulq Width(%rip), %rcx
	movq %rcx, %rax
	addq -8(%rbp), %rax
	movq %rax, %rcx
	imulq $1, %rcx
	movq %rbx, %rax
	addq %rcx, %rax
	movzbq (%rax), %rcx
	movq %rcx, %rbx
	cmpq $32, %rbx
	sete %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable0
	jmp .lable1
.lable0:
	movq $42, %rbx
	movb %bl, (%rax)
	jmp .lable2
.lable1:
	call new_food
.lable2:
	leave
	retq
	.global init
	.text
init:
	pushq %rbp
	movq %rsp, %rbp
	subq $40, %rsp
	movq $0, -8(%rbp)
.lable3:
	movq -8(%rbp), %rax
	cmpq snake_len(%rip), %rax
	setl %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable6
	jmp .lable4
.lable6:
	movq $2, %rbx
	movq Width(%rip), %rax
	cqto
	idivq %rbx
	movq %rax, %rax
	movq %rax, %rbx
	addq -8(%rbp), %rbx
	leaq snake_x(%rip), %rax
	movq -8(%rbp), %rcx
	imulq $8, %rcx
	movq %rax, %rdx
	addq %rcx, %rdx
	movq %rbx, (%rdx)
	movq $2, %rbx
	movq Height(%rip), %rax
	cqto
	idivq %rbx
	movq %rax, %rdx
	leaq snake_y(%rip), %rbx
	movq %rbx, %rax
	addq %rcx, %rax
	movq %rdx, (%rax)
	movq -8(%rbp), %rax
	addq $1, %rax
	movq %rax, -8(%rbp)
	jmp .lable3
.lable4:
	movq $0, -8(%rbp)
.lable7:
	movq -8(%rbp), %rax
	cmpq Height(%rip), %rax
	setl %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable10
	jmp .lable8
.lable10:
	movq $0, -16(%rbp)
.lable11:
	movq -16(%rbp), %rax
	cmpq Width(%rip), %rax
	setl %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable14
	jmp .lable12
.lable14:
	leaq Map(%rip), %rax
	movq -8(%rbp), %rdx
	imulq Width(%rip), %rdx
	movq %rdx, %rbx
	addq -16(%rbp), %rbx
	movq %rbx, %rdx
	imulq $1, %rdx
	movq %rax, %rbx
	addq %rdx, %rbx
	movq %rbx, -24(%rbp)
	movq -16(%rbp), %rbx
	cmpq $0, %rbx
	sete %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable15
	movq Width(%rip), %rbx
	subq $1, %rbx
	movq -16(%rbp), %rdx
	cmpq %rbx, %rdx
	sete %dl
	movzbq %dl, %rdx
	testq %rdx, %rdx
	jnz .lable15
	movq -8(%rbp), %rdx
	cmpq $0, %rdx
	sete %dl
	movzbq %dl, %rdx
	testq %rdx, %rdx
	jnz .lable15
	movq Height(%rip), %rdx
	subq $1, %rdx
	movq -8(%rbp), %rbx
	cmpq %rdx, %rbx
	sete %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable15
	jmp .lable16
.lable15:
	movq $1, %rbx
	jmp .lable17
.lable16:
	movq $0, %rbx
.lable17:
	testq %rbx, %rbx
	jnz .lable18
	jmp .lable19
.lable18:
	movq -24(%rbp), %rbx
	movq $35, %rdx
	movb %dl, (%rbx)
	jmp .lable20
.lable19:
	movq -24(%rbp), %rdx
	movq $32, %rbx
	movb %bl, (%rdx)
.lable20:
	movq -16(%rbp), %rbx
	addq $1, %rbx
	movq %rbx, -16(%rbp)
	jmp .lable11
.lable12:
	movq -8(%rbp), %rbx
	addq $1, %rbx
	movq %rbx, -8(%rbp)
	jmp .lable7
.lable8:
	movq $0, -8(%rbp)
.lable21:
	movq -8(%rbp), %rbx
	cmpq snake_len(%rip), %rbx
	setl %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable24
	jmp .lable22
.lable24:
	leaq snake_x(%rip), %rbx
	movq -8(%rbp), %rdx
	imulq $8, %rdx
	movq %rbx, %rax
	addq %rdx, %rax
	movq (%rax), %rbx
	movq %rbx, -32(%rbp)
	leaq snake_y(%rip), %rbx
	movq %rbx, %rax
	addq %rdx, %rax
	movq (%rax), %rbx
	movq %rbx, -40(%rbp)
	movq -8(%rbp), %rbx
	cmpq $0, %rbx
	sete %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable25
	jmp .lable26
.lable25:
	leaq Map(%rip), %rbx
	movq -40(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, %rdx
	addq -32(%rbp), %rdx
	movq %rdx, %rax
	imulq $1, %rax
	movq %rbx, %rdx
	addq %rax, %rdx
	movq $64, %rax
	movb %al, (%rdx)
	jmp .lable27
.lable26:
	leaq Map(%rip), %rdx
	movq -40(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, %rbx
	addq -32(%rbp), %rbx
	movq %rbx, %rax
	imulq $1, %rax
	movq %rdx, %rbx
	addq %rax, %rbx
	movq $43, %rax
	movb %al, (%rbx)
.lable27:
	movq -8(%rbp), %rbx
	addq $1, %rbx
	movq %rbx, -8(%rbp)
	jmp .lable21
.lable22:
	pushq $0
	call time
	addq $8, %rsp
	movq %rax, %rbx
	pushq %rbx
	call srand
	addq $8, %rsp
	call new_food
	leave
	retq
	.global draw
	.text
draw:
	pushq %rbp
	movq %rsp, %rbp
	subq $24, %rsp
	leaq .str0(%rip), %rbx
	pushq %rbx
	call system
	addq $8, %rsp
	movq $0, -8(%rbp)
.lable28:
	movq -8(%rbp), %rbx
	cmpq Height(%rip), %rbx
	setl %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable31
	jmp .lable29
.lable31:
	movq $0, -16(%rbp)
.lable32:
	movq -16(%rbp), %rbx
	cmpq Width(%rip), %rbx
	setl %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable35
	jmp .lable33
.lable35:
	leaq Map(%rip), %rbx
	movq -8(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, %rdx
	addq -16(%rbp), %rdx
	movq %rdx, %rax
	imulq $1, %rax
	movq %rbx, %rdx
	addq %rax, %rdx
	movq %rdx, -24(%rbp)
	movq -24(%rbp), %rax
	movzbq (%rax), %rdx
	pushq %rdx
	call putchar
	addq $8, %rsp
	movq -16(%rbp), %rdx
	addq $1, %rdx
	movq %rdx, -16(%rbp)
	jmp .lable32
.lable33:
	pushq $10
	call putchar
	addq $8, %rsp
	movq -8(%rbp), %rdx
	addq $1, %rdx
	movq %rdx, -8(%rbp)
	jmp .lable28
.lable29:
	leave
	retq
	.global update
	.text
update:
	pushq %rbp
	movq %rsp, %rbp
	subq $49, %rsp
	leaq snake_x(%rip), %rdx
	movq snake_len(%rip), %rax
	subq $1, %rax
	movq %rax, %rbx
	imulq $8, %rbx
	movq %rdx, %rax
	addq %rbx, %rax
	movq (%rax), %rcx
	movq %rcx, -16(%rbp)
	leaq snake_y(%rip), %rcx
	movq %rcx, %rax
	addq %rbx, %rax
	movq (%rax), %rbx
	movq %rbx, -24(%rbp)
	movq (%rdx), %rbx
	movq %rbx, -32(%rbp)
	movq (%rcx), %rbx
	movq %rbx, -40(%rbp)
	movq grown(%rip), %rbx
	testq %rbx, %rbx
	jnz .lable36
	jmp .lable37
.lable36:
	movq $0, grown(%rip)
	movq snake_len(%rip), %rbx
	addq $1, %rbx
	movq %rbx, snake_len(%rip)
	jmp .lable38
.lable37:
	leaq Map(%rip), %rbx
	movq -24(%rbp), %rcx
	imulq Width(%rip), %rcx
	movq %rcx, %rdx
	addq -16(%rbp), %rdx
	movq %rdx, %rcx
	imulq $1, %rcx
	movq %rbx, %rdx
	addq %rcx, %rdx
	movq $32, %rcx
	movb %cl, (%rdx)
.lable38:
	leaq Map(%rip), %rdx
	movq -40(%rbp), %rcx
	imulq Width(%rip), %rcx
	movq %rcx, %rbx
	addq -32(%rbp), %rbx
	movq %rbx, %rcx
	imulq $1, %rcx
	movq %rdx, %rbx
	addq %rcx, %rbx
	movq $43, %rcx
	movb %cl, (%rbx)
	movq snake_len(%rip), %rbx
	subq $1, %rbx
	movq %rbx, -8(%rbp)
.lable39:
	movq -8(%rbp), %rbx
	cmpq $0, %rbx
	setg %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable42
	jmp .lable40
.lable42:
	leaq snake_x(%rip), %rbx
	movq -8(%rbp), %rcx
	subq $1, %rcx
	movq %rcx, %rdx
	imulq $8, %rdx
	movq %rbx, %rax
	addq %rdx, %rax
	movq -8(%rbp), %rsi
	imulq $8, %rsi
	movq %rbx, %rdi
	addq %rsi, %rdi
	movq (%rax), %rbx
	movq %rbx, (%rdi)
	leaq snake_y(%rip), %rbx
	movq %rbx, %rdi
	addq %rdx, %rdi
	movq %rbx, %rdx
	addq %rsi, %rdx
	movq (%rdi), %rbx
	movq %rbx, (%rdx)
	movq %rcx, -8(%rbp)
	jmp .lable39
.lable40:
	movq dir(%rip), %rcx
	cmpq $0, %rcx
	sete %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable43
	jmp .lable44
.lable43:
	leaq snake_x(%rip), %rcx
	movq (%rcx), %rbx
	movq %rbx, %rdx
	subq $1, %rdx
	movq %rdx, (%rcx)
	jmp .lable51
.lable44:
	movq dir(%rip), %rdx
	cmpq $1, %rdx
	sete %dl
	movzbq %dl, %rdx
	testq %rdx, %rdx
	jnz .lable46
	jmp .lable47
.lable46:
	leaq snake_x(%rip), %rdx
	movq (%rdx), %rcx
	movq %rcx, %rbx
	addq $1, %rbx
	movq %rbx, (%rdx)
	jmp .lable51
.lable47:
	movq dir(%rip), %rbx
	cmpq $2, %rbx
	sete %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable49
	jmp .lable50
.lable49:
	leaq snake_y(%rip), %rbx
	movq (%rbx), %rdx
	movq %rdx, %rcx
	subq $1, %rcx
	movq %rcx, (%rbx)
	jmp .lable51
.lable50:
	leaq snake_y(%rip), %rcx
	movq (%rcx), %rbx
	movq %rbx, %rdx
	addq $1, %rdx
	movq %rdx, (%rcx)
.lable51:
	leaq snake_x(%rip), %rdx
	movq (%rdx), %rcx
	movq %rcx, -32(%rbp)
	leaq snake_y(%rip), %rcx
	movq (%rcx), %rdx
	movq %rdx, -40(%rbp)
	leaq Map(%rip), %rdx
	movq -40(%rbp), %rcx
	imulq Width(%rip), %rcx
	movq %rcx, %rbx
	addq -32(%rbp), %rbx
	movq %rbx, %rcx
	imulq $1, %rcx
	movq %rdx, %rbx
	addq %rcx, %rbx
	movq %rbx, -48(%rbp)
	movq -48(%rbp), %rcx
	movzbq (%rcx), %rbx
	movq %rbx, %rcx
	cmpq $35, %rcx
	sete %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable52
	movq %rbx, %rcx
	cmpq $43, %rcx
	sete %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable52
	jmp .lable53
.lable52:
	movq $1, %rcx
	jmp .lable54
.lable53:
	movq $0, %rcx
.lable54:
	testq %rcx, %rcx
	jnz .lable55
	jmp .lable56
.lable55:
	movq $1, exit(%rip)
	jmp .lable84
.lable56:
	movq -48(%rbp), %rbx
	movzbq (%rbx), %rcx
	movq %rcx, %rbx
	cmpq $42, %rbx
	sete %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable58
	jmp .lable59
.lable58:
	movq $1, grown(%rip)
	call new_food
.lable59:
	movq -48(%rbp), %rbx
	movq $64, %rcx
	movb %cl, (%rbx)
	call kbhit
	movq %rax, %rbx
	testq %rbx, %rbx
	jnz .lable60
	jmp .lable84
.lable60:
	call getch
	movq %rax, %rbx
	movb %bl, -49(%rbp)
	movsbq -49(%rbp), %rbx
	cmpq $97, %rbx
	sete %bl
	movzbq %bl, %rbx
	movq %rbx, %rcx
	testq %rcx, %rcx
	setz %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable62
	movq dir(%rip), %rcx
	cmpq $1, %rcx
	setne %cl
	movzbq %cl, %rcx
	movq %rcx, %rbx
	testq %rbx, %rbx
	setz %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable62
	jmp .lable63
.lable62:
	movq $0, %rbx
	jmp .lable64
.lable63:
	movq $1, %rbx
.lable64:
	testq %rbx, %rbx
	jnz .lable65
	jmp .lable66
.lable65:
	movq $0, dir(%rip)
	jmp .lable84
.lable66:
	movsbq -49(%rbp), %rbx
	cmpq $100, %rbx
	sete %bl
	movzbq %bl, %rbx
	movq %rbx, %rcx
	testq %rcx, %rcx
	setz %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable68
	movq dir(%rip), %rcx
	cmpq $0, %rcx
	setne %cl
	movzbq %cl, %rcx
	movq %rcx, %rbx
	testq %rbx, %rbx
	setz %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable68
	jmp .lable69
.lable68:
	movq $0, %rbx
	jmp .lable70
.lable69:
	movq $1, %rbx
.lable70:
	testq %rbx, %rbx
	jnz .lable71
	jmp .lable72
.lable71:
	movq $1, dir(%rip)
	jmp .lable84
.lable72:
	movsbq -49(%rbp), %rbx
	cmpq $119, %rbx
	sete %bl
	movzbq %bl, %rbx
	movq %rbx, %rcx
	testq %rcx, %rcx
	setz %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable74
	movq dir(%rip), %rcx
	cmpq $3, %rcx
	setne %cl
	movzbq %cl, %rcx
	movq %rcx, %rbx
	testq %rbx, %rbx
	setz %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable74
	jmp .lable75
.lable74:
	movq $0, %rbx
	jmp .lable76
.lable75:
	movq $1, %rbx
.lable76:
	testq %rbx, %rbx
	jnz .lable77
	jmp .lable78
.lable77:
	movq $2, dir(%rip)
	jmp .lable84
.lable78:
	movsbq -49(%rbp), %rbx
	cmpq $115, %rbx
	sete %bl
	movzbq %bl, %rbx
	movq %rbx, %rcx
	testq %rcx, %rcx
	setz %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable80
	movq dir(%rip), %rcx
	cmpq $2, %rcx
	setne %cl
	movzbq %cl, %rcx
	movq %rcx, %rbx
	testq %rbx, %rbx
	setz %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable80
	jmp .lable81
.lable80:
	movq $0, %rbx
	jmp .lable82
.lable81:
	movq $1, %rbx
.lable82:
	testq %rbx, %rbx
	jnz .lable83
	jmp .lable84
.lable83:
	movq $3, dir(%rip)
.lable84:
	leave
	retq
	.global main
	.text
main:
	pushq %rbp
	movq %rsp, %rbp
	call init
.lable85:
	movq exit(%rip), %rbx
	testq %rbx, %rbx
	jnz .lable86
	call draw
	call update
	pushq $100000
	call usleep
	addq $8, %rsp
	jmp .lable85
.lable86:
	movq $0, %rax
	leave
	retq
	.global _start
	.text
_start:
	call main
	movq $60, %rax
	xorq %rdi, %rdi
	syscall

