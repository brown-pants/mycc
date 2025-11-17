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
	movq -8(%rbp), %rcx
	imulq $8, %rcx
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
	movq %rdx, %rcx
	addq -16(%rbp), %rcx
	movq %rcx, %rdx
	imulq $1, %rdx
	movq %rax, %rcx
	addq %rdx, %rcx
	movq %rcx, -24(%rbp)
	movq -16(%rbp), %rcx
	cmpq $0, %rcx
	sete %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable15
	movq Width(%rip), %rcx
	subq $1, %rcx
	movq -16(%rbp), %rdx
	cmpq %rcx, %rdx
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
	movq -8(%rbp), %rcx
	cmpq %rdx, %rcx
	sete %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable15
	jmp .lable16
.lable15:
	movq $1, %rcx
	jmp .lable17
.lable16:
	movq $0, %rcx
.lable17:
	testq %rcx, %rcx
	jnz .lable18
	jmp .lable19
.lable18:
	movq -24(%rbp), %rcx
	movq $35, %rdx
	movb %dl, (%rcx)
	jmp .lable20
.lable19:
	movq -24(%rbp), %rdx
	movq $32, %rcx
	movb %cl, (%rdx)
.lable20:
	movq -16(%rbp), %rcx
	addq $1, %rcx
	movq %rcx, -16(%rbp)
	jmp .lable11
.lable12:
	movq -8(%rbp), %rcx
	addq $1, %rcx
	movq %rcx, -8(%rbp)
	jmp .lable7
.lable8:
	movq $0, -8(%rbp)
.lable21:
	movq -8(%rbp), %rcx
	cmpq snake_len(%rip), %rcx
	setl %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable24
	jmp .lable22
.lable24:
	leaq snake_x(%rip), %rcx
	movq -8(%rbp), %rdx
	imulq $8, %rdx
	movq %rcx, %rax
	addq %rdx, %rax
	movq (%rax), %rcx
	movq %rcx, -32(%rbp)
	leaq snake_y(%rip), %rcx
	movq %rcx, %rax
	addq %rdx, %rax
	movq (%rax), %rcx
	movq %rcx, -40(%rbp)
	movq -8(%rbp), %rcx
	cmpq $0, %rcx
	sete %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable25
	jmp .lable26
.lable25:
	leaq Map(%rip), %rcx
	movq -40(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, %rdx
	addq -32(%rbp), %rdx
	movq %rdx, %rax
	imulq $1, %rax
	movq %rcx, %rdx
	addq %rax, %rdx
	movq $64, %rax
	movb %al, (%rdx)
	jmp .lable27
.lable26:
	leaq Map(%rip), %rdx
	movq -40(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, %rcx
	addq -32(%rbp), %rcx
	movq %rcx, %rax
	imulq $1, %rax
	movq %rdx, %rcx
	addq %rax, %rcx
	movq $43, %rax
	movb %al, (%rcx)
.lable27:
	movq -8(%rbp), %rcx
	addq $1, %rcx
	movq %rcx, -8(%rbp)
	jmp .lable21
.lable22:
	pushq $0
	call time
	addq $8, %rsp
	movq %rax, %rcx
	pushq %rcx
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
	leaq .str0(%rip), %rcx
	pushq %rcx
	call system
	addq $8, %rsp
	movq $0, -8(%rbp)
.lable28:
	movq -8(%rbp), %rcx
	cmpq Height(%rip), %rcx
	setl %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable31
	jmp .lable29
.lable31:
	movq $0, -16(%rbp)
.lable32:
	movq -16(%rbp), %rcx
	cmpq Width(%rip), %rcx
	setl %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable35
	jmp .lable33
.lable35:
	leaq Map(%rip), %rcx
	movq -8(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, %rdx
	addq -16(%rbp), %rdx
	movq %rdx, %rax
	imulq $1, %rax
	movq %rcx, %rdx
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
	movq %rax, %rcx
	imulq $8, %rcx
	movq %rdx, %rax
	addq %rcx, %rax
	movq (%rax), %rbx
	movq %rbx, -16(%rbp)
	leaq snake_y(%rip), %rbx
	movq %rbx, %rax
	addq %rcx, %rax
	movq (%rax), %rcx
	movq %rcx, -24(%rbp)
	movq (%rdx), %rcx
	movq %rcx, -32(%rbp)
	movq (%rbx), %rcx
	movq %rcx, -40(%rbp)
	movq grown(%rip), %rcx
	testq %rcx, %rcx
	jnz .lable36
	jmp .lable37
.lable36:
	movq $0, grown(%rip)
	movq snake_len(%rip), %rcx
	addq $1, %rcx
	movq %rcx, snake_len(%rip)
	jmp .lable38
.lable37:
	leaq Map(%rip), %rcx
	movq -24(%rbp), %rbx
	imulq Width(%rip), %rbx
	movq %rbx, %rdx
	addq -16(%rbp), %rdx
	movq %rdx, %rbx
	imulq $1, %rbx
	movq %rcx, %rdx
	addq %rbx, %rdx
	movq $32, %rbx
	movb %bl, (%rdx)
.lable38:
	leaq Map(%rip), %rdx
	movq -40(%rbp), %rbx
	imulq Width(%rip), %rbx
	movq %rbx, %rcx
	addq -32(%rbp), %rcx
	movq %rcx, %rbx
	imulq $1, %rbx
	movq %rdx, %rcx
	addq %rbx, %rcx
	movq $43, %rbx
	movb %bl, (%rcx)
	movq snake_len(%rip), %rcx
	subq $1, %rcx
	movq %rcx, -8(%rbp)
.lable39:
	movq -8(%rbp), %rcx
	cmpq $0, %rcx
	setg %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable42
	jmp .lable40
.lable42:
	leaq snake_x(%rip), %rcx
	movq -8(%rbp), %rbx
	subq $1, %rbx
	movq %rbx, %rdx
	imulq $8, %rdx
	movq %rcx, %rbx
	addq %rdx, %rbx
	movq -8(%rbp), %rdx
	imulq $8, %rdx
	movq %rcx, %rax
	addq %rdx, %rax
	movq (%rbx), %rdx
	movq %rdx, (%rax)
	leaq snake_y(%rip), %rdx
	movq -8(%rbp), %rax
	subq $1, %rax
	movq %rax, %rbx
	imulq $8, %rbx
	movq %rdx, %rax
	addq %rbx, %rax
	movq -8(%rbp), %rbx
	imulq $8, %rbx
	movq %rdx, %rcx
	addq %rbx, %rcx
	movq (%rax), %rbx
	movq %rbx, (%rcx)
	movq -8(%rbp), %rbx
	subq $1, %rbx
	movq %rbx, -8(%rbp)
	jmp .lable39
.lable40:
	movq dir(%rip), %rbx
	cmpq $0, %rbx
	sete %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable43
	jmp .lable44
.lable43:
	leaq snake_x(%rip), %rbx
	movq (%rbx), %rcx
	movq %rcx, %rax
	subq $1, %rax
	movq %rax, (%rbx)
	jmp .lable51
.lable44:
	movq dir(%rip), %rax
	cmpq $1, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable46
	jmp .lable47
.lable46:
	leaq snake_x(%rip), %rax
	movq (%rax), %rbx
	movq %rbx, %rcx
	addq $1, %rcx
	movq %rcx, (%rax)
	jmp .lable51
.lable47:
	movq dir(%rip), %rcx
	cmpq $2, %rcx
	sete %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable49
	jmp .lable50
.lable49:
	leaq snake_y(%rip), %rcx
	movq (%rcx), %rax
	movq %rax, %rbx
	subq $1, %rbx
	movq %rbx, (%rcx)
	jmp .lable51
.lable50:
	leaq snake_y(%rip), %rbx
	movq (%rbx), %rcx
	movq %rcx, %rax
	addq $1, %rax
	movq %rax, (%rbx)
.lable51:
	leaq snake_x(%rip), %rax
	movq (%rax), %rbx
	movq %rbx, -32(%rbp)
	leaq snake_y(%rip), %rbx
	movq (%rbx), %rax
	movq %rax, -40(%rbp)
	leaq Map(%rip), %rax
	movq -40(%rbp), %rbx
	imulq Width(%rip), %rbx
	movq %rbx, %rcx
	addq -32(%rbp), %rcx
	movq %rcx, %rbx
	imulq $1, %rbx
	movq %rax, %rcx
	addq %rbx, %rcx
	movq %rcx, -48(%rbp)
	movq -48(%rbp), %rbx
	movzbq (%rbx), %rcx
	movq %rcx, %rbx
	cmpq $35, %rbx
	sete %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable52
	movq %rcx, %rbx
	cmpq $43, %rbx
	sete %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable52
	jmp .lable53
.lable52:
	movq $1, %rbx
	jmp .lable54
.lable53:
	movq $0, %rbx
.lable54:
	testq %rbx, %rbx
	jnz .lable55
	jmp .lable56
.lable55:
	movq $1, exit(%rip)
	jmp .lable84
.lable56:
	movq -48(%rbp), %rcx
	movzbq (%rcx), %rbx
	movq %rbx, %rcx
	cmpq $42, %rcx
	sete %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable58
	jmp .lable59
.lable58:
	movq $1, grown(%rip)
	call new_food
.lable59:
	movq -48(%rbp), %rcx
	movq $64, %rbx
	movb %bl, (%rcx)
	call kbhit
	movq %rax, %rcx
	testq %rcx, %rcx
	jnz .lable60
	jmp .lable84
.lable60:
	call getch
	movq %rax, %rcx
	movb %cl, -49(%rbp)
	movsbq -49(%rbp), %rcx
	cmpq $97, %rcx
	sete %cl
	movzbq %cl, %rcx
	movq %rcx, %rbx
	testq %rbx, %rbx
	setz %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable62
	movq dir(%rip), %rbx
	cmpq $1, %rbx
	setne %bl
	movzbq %bl, %rbx
	movq %rbx, %rcx
	testq %rcx, %rcx
	setz %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable62
	jmp .lable63
.lable62:
	movq $0, %rcx
	jmp .lable64
.lable63:
	movq $1, %rcx
.lable64:
	testq %rcx, %rcx
	jnz .lable65
	jmp .lable66
.lable65:
	movq $0, dir(%rip)
	jmp .lable84
.lable66:
	movsbq -49(%rbp), %rcx
	cmpq $100, %rcx
	sete %cl
	movzbq %cl, %rcx
	movq %rcx, %rbx
	testq %rbx, %rbx
	setz %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable68
	movq dir(%rip), %rbx
	cmpq $0, %rbx
	setne %bl
	movzbq %bl, %rbx
	movq %rbx, %rcx
	testq %rcx, %rcx
	setz %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable68
	jmp .lable69
.lable68:
	movq $0, %rcx
	jmp .lable70
.lable69:
	movq $1, %rcx
.lable70:
	testq %rcx, %rcx
	jnz .lable71
	jmp .lable72
.lable71:
	movq $1, dir(%rip)
	jmp .lable84
.lable72:
	movsbq -49(%rbp), %rcx
	cmpq $119, %rcx
	sete %cl
	movzbq %cl, %rcx
	movq %rcx, %rbx
	testq %rbx, %rbx
	setz %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable74
	movq dir(%rip), %rbx
	cmpq $3, %rbx
	setne %bl
	movzbq %bl, %rbx
	movq %rbx, %rcx
	testq %rcx, %rcx
	setz %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable74
	jmp .lable75
.lable74:
	movq $0, %rcx
	jmp .lable76
.lable75:
	movq $1, %rcx
.lable76:
	testq %rcx, %rcx
	jnz .lable77
	jmp .lable78
.lable77:
	movq $2, dir(%rip)
	jmp .lable84
.lable78:
	movsbq -49(%rbp), %rcx
	cmpq $115, %rcx
	sete %cl
	movzbq %cl, %rcx
	movq %rcx, %rbx
	testq %rbx, %rbx
	setz %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable80
	movq dir(%rip), %rbx
	cmpq $2, %rbx
	setne %bl
	movzbq %bl, %rbx
	movq %rbx, %rcx
	testq %rcx, %rcx
	setz %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable80
	jmp .lable81
.lable80:
	movq $0, %rcx
	jmp .lable82
.lable81:
	movq $1, %rcx
.lable82:
	testq %rcx, %rcx
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
	movq exit(%rip), %rcx
	testq %rcx, %rcx
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

