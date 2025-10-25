	.data
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
	pushq %rcx
	movq %rbx, %rax
	cqto
	movq %rdi, %rcx
	idivq %rcx
	movq %rdx, %rcx
	addq $8, %rsp
	popq %rax
	movq %rcx, -8(%rbp)
	call rand
	movq %rax, %rcx
	movq %rcx, %rax
	addq $1, %rax
	movq Height(%rip), %rcx
	subq $1, %rcx
	pushq %rax
	pushq %rcx
	movq %rax, %rax
	cqto
	movq %rcx, %rcx
	idivq %rcx
	movq %rdx, %rbx
	popq %rcx
	popq %rax
	movq %rbx, -16(%rbp)
	movq -16(%rbp), %rbx
	imulq Width(%rip), %rbx
	movq %rbx, %rcx
	addq -8(%rbp), %rcx
	movq %rcx, %rbx
	imulq $1, %rbx
	leaq Map(%rip), %rcx
	movq %rcx, %rax
	addq %rbx, %rax
	movzbq (%rax), %rcx
	movq %rcx, %rax
	cmpq $32, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable0
	jmp .lable1
.lable0:
	movq -16(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, %rcx
	addq -8(%rbp), %rcx
	movq %rcx, %rax
	imulq $1, %rax
	leaq Map(%rip), %rcx
	movq %rcx, %rbx
	addq %rax, %rbx
	movq $42, %rcx
	movb %cl, (%rbx)
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
	movq -8(%rbp), %rbx
	cmpq snake_len(%rip), %rbx
	setl %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable6
	jmp .lable4
.lable6:
	movq Width(%rip), %rax
	cqto
	movq $2, %rcx
	idivq %rcx
	movq %rax, %rbx
	movq %rbx, %rcx
	addq -8(%rbp), %rcx
	movq -8(%rbp), %rbx
	imulq $8, %rbx
	leaq snake_x(%rip), %rax
	movq %rax, %rdx
	addq %rbx, %rdx
	movq %rcx, (%rdx)
	pushq %rdx
	movq Height(%rip), %rax
	cqto
	movq $2, %rcx
	idivq %rcx
	movq %rax, %rdx
	addq $8, %rsp
	movq -8(%rbp), %rcx
	imulq $8, %rcx
	leaq snake_y(%rip), %rax
	movq %rax, %rbx
	addq %rcx, %rbx
	movq %rdx, (%rbx)
	movq -8(%rbp), %rbx
	addq $1, %rbx
	movq %rbx, -8(%rbp)
	jmp .lable3
.lable4:
	movq $0, -8(%rbp)
.lable7:
	movq -8(%rbp), %rbx
	cmpq Height(%rip), %rbx
	setl %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable10
	jmp .lable8
.lable10:
	movq $0, -16(%rbp)
.lable11:
	movq -16(%rbp), %rbx
	cmpq Width(%rip), %rbx
	setl %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable14
	jmp .lable12
.lable14:
	movq -8(%rbp), %rbx
	imulq Width(%rip), %rbx
	movq %rbx, %rdx
	addq -16(%rbp), %rdx
	movq %rdx, %rbx
	imulq $1, %rbx
	leaq Map(%rip), %rdx
	movq %rdx, %rax
	addq %rbx, %rax
	movq %rax, -24(%rbp)
	movq -16(%rbp), %rax
	cmpq $0, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable15
	movq Width(%rip), %rax
	subq $1, %rax
	movq -16(%rbp), %rdx
	cmpq %rax, %rdx
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
	movq -8(%rbp), %rax
	cmpq %rdx, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable15
	jmp .lable16
.lable15:
	movq $1, %rax
	jmp .lable17
.lable16:
	movq $0, %rax
.lable17:
	testq %rax, %rax
	jnz .lable18
	jmp .lable19
.lable18:
	movq -24(%rbp), %rax
	movq $35, %rdx
	movb %dl, (%rax)
	jmp .lable20
.lable19:
	movq -24(%rbp), %rdx
	movq $32, %rax
	movb %al, (%rdx)
.lable20:
	movq -16(%rbp), %rax
	addq $1, %rax
	movq %rax, -16(%rbp)
	jmp .lable11
.lable12:
	movq -8(%rbp), %rax
	addq $1, %rax
	movq %rax, -8(%rbp)
	jmp .lable7
.lable8:
	movq $0, -8(%rbp)
.lable21:
	movq -8(%rbp), %rax
	cmpq snake_len(%rip), %rax
	setl %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable24
	jmp .lable22
.lable24:
	movq -8(%rbp), %rax
	imulq $8, %rax
	leaq snake_x(%rip), %rdx
	movq %rdx, %rbx
	addq %rax, %rbx
	movq (%rbx), %rdx
	movq %rdx, -32(%rbp)
	movq -8(%rbp), %rdx
	imulq $8, %rdx
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
	movq -40(%rbp), %rbx
	imulq Width(%rip), %rbx
	movq %rbx, %rax
	addq -32(%rbp), %rax
	movq %rax, %rbx
	imulq $1, %rbx
	leaq Map(%rip), %rax
	movq %rax, %rdx
	addq %rbx, %rdx
	movq $64, %rax
	movb %al, (%rdx)
	jmp .lable27
.lable26:
	movq -40(%rbp), %rdx
	imulq Width(%rip), %rdx
	movq %rdx, %rax
	addq -32(%rbp), %rax
	movq %rax, %rdx
	imulq $1, %rdx
	leaq Map(%rip), %rax
	movq %rax, %rbx
	addq %rdx, %rbx
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
	movq -8(%rbp), %rbx
	imulq Width(%rip), %rbx
	movq %rbx, %rax
	addq -16(%rbp), %rax
	movq %rax, %rbx
	imulq $1, %rbx
	leaq Map(%rip), %rax
	movq %rax, %rdx
	addq %rbx, %rdx
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
	movq snake_len(%rip), %rdx
	subq $1, %rdx
	movq %rdx, %rax
	imulq $8, %rax
	leaq snake_x(%rip), %rdx
	movq %rdx, %rbx
	addq %rax, %rbx
	movq (%rbx), %rdx
	movq %rdx, -16(%rbp)
	movq snake_len(%rip), %rdx
	subq $1, %rdx
	movq %rdx, %rbx
	imulq $8, %rbx
	leaq snake_y(%rip), %rdx
	movq %rdx, %rax
	addq %rbx, %rax
	movq (%rax), %rdx
	movq %rdx, -24(%rbp)
	movq snake_x(%rip), %rdx
	movq %rdx, -32(%rbp)
	movq snake_y(%rip), %rdx
	movq %rdx, -40(%rbp)
	movq grown(%rip), %rdx
	testq %rdx, %rdx
	jnz .lable36
	jmp .lable37
.lable36:
	movq $0, grown(%rip)
	movq snake_len(%rip), %rdx
	addq $1, %rdx
	movq %rdx, snake_len(%rip)
	jmp .lable38
.lable37:
	movq -24(%rbp), %rdx
	imulq Width(%rip), %rdx
	movq %rdx, %rax
	addq -16(%rbp), %rax
	movq %rax, %rdx
	imulq $1, %rdx
	leaq Map(%rip), %rax
	movq %rax, %rbx
	addq %rdx, %rbx
	movq $32, %rax
	movb %al, (%rbx)
.lable38:
	movq -40(%rbp), %rbx
	imulq Width(%rip), %rbx
	movq %rbx, %rax
	addq -32(%rbp), %rax
	movq %rax, %rbx
	imulq $1, %rbx
	leaq Map(%rip), %rax
	movq %rax, %rdx
	addq %rbx, %rdx
	movq $43, %rax
	movb %al, (%rdx)
	movq snake_len(%rip), %rdx
	subq $1, %rdx
	movq %rdx, -8(%rbp)
.lable39:
	movq -8(%rbp), %rdx
	cmpq $0, %rdx
	setg %dl
	movzbq %dl, %rdx
	testq %rdx, %rdx
	jnz .lable42
	jmp .lable40
.lable42:
	movq -8(%rbp), %rdx
	subq $1, %rdx
	movq %rdx, %rax
	imulq $8, %rax
	leaq snake_x(%rip), %rdx
	movq %rdx, %rbx
	addq %rax, %rbx
	movq -8(%rbp), %rdx
	imulq $8, %rdx
	leaq snake_x(%rip), %rax
	movq %rax, %rcx
	addq %rdx, %rcx
	movq (%rbx), %rax
	movq %rax, (%rcx)
	movq -8(%rbp), %rax
	subq $1, %rax
	movq %rax, %rcx
	imulq $8, %rcx
	leaq snake_y(%rip), %rax
	movq %rax, %rbx
	addq %rcx, %rbx
	movq -8(%rbp), %rax
	imulq $8, %rax
	leaq snake_y(%rip), %rcx
	movq %rcx, %rdx
	addq %rax, %rdx
	movq (%rbx), %rcx
	movq %rcx, (%rdx)
	movq -8(%rbp), %rcx
	subq $1, %rcx
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
	movq snake_x(%rip), %rcx
	subq $1, %rcx
	movq %rcx, snake_x(%rip)
	jmp .lable51
.lable44:
	movq dir(%rip), %rcx
	cmpq $1, %rcx
	sete %cl
	movzbq %cl, %rcx
	testq %rcx, %rcx
	jnz .lable46
	jmp .lable47
.lable46:
	movq snake_x(%rip), %rcx
	addq $1, %rcx
	movq %rcx, snake_x(%rip)
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
	movq snake_y(%rip), %rcx
	subq $1, %rcx
	movq %rcx, snake_y(%rip)
	jmp .lable51
.lable50:
	movq snake_y(%rip), %rcx
	addq $1, %rcx
	movq %rcx, snake_y(%rip)
.lable51:
	movq snake_x(%rip), %rcx
	movq %rcx, -32(%rbp)
	movq snake_y(%rip), %rcx
	movq %rcx, -40(%rbp)
	movq -40(%rbp), %rcx
	imulq Width(%rip), %rcx
	movq %rcx, %rdx
	addq -32(%rbp), %rdx
	movq %rdx, %rcx
	imulq $1, %rcx
	leaq Map(%rip), %rdx
	movq %rdx, %rbx
	addq %rcx, %rbx
	movq %rbx, -48(%rbp)
	movq -48(%rbp), %rdx
	movzbq (%rdx), %rbx
	movq %rbx, %rdx
	cmpq $35, %rdx
	sete %dl
	movzbq %dl, %rdx
	testq %rdx, %rdx
	jnz .lable52
	movq -48(%rbp), %rbx
	movzbq (%rbx), %rdx
	movq %rdx, %rbx
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
	movq -48(%rbp), %rdx
	movzbq (%rdx), %rbx
	movq %rbx, %rdx
	cmpq $42, %rdx
	sete %dl
	movzbq %dl, %rdx
	testq %rdx, %rdx
	jnz .lable58
	jmp .lable59
.lable58:
	movq $1, grown(%rip)
	call new_food
.lable59:
	movq -48(%rbp), %rdx
	movq $64, %rbx
	movb %bl, (%rdx)
	call kbhit
	movq %rax, %rdx
	testq %rdx, %rdx
	jnz .lable60
	jmp .lable84
.lable60:
	call getch
	movq %rax, %rdx
	movb %dl, -49(%rbp)
	movsbq -49(%rbp), %rdx
	cmpq $97, %rdx
	sete %dl
	movzbq %dl, %rdx
	movq %rdx, %rbx
	testq %rbx, %rbx
	setz %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable62
	movq dir(%rip), %rbx
	cmpq $1, %rbx
	setne %bl
	movzbq %bl, %rbx
	movq %rbx, %rdx
	testq %rdx, %rdx
	setz %dl
	movzbq %dl, %rdx
	testq %rdx, %rdx
	jnz .lable62
	jmp .lable63
.lable62:
	movq $0, %rdx
	jmp .lable64
.lable63:
	movq $1, %rdx
.lable64:
	testq %rdx, %rdx
	jnz .lable65
	jmp .lable66
.lable65:
	movq $0, dir(%rip)
	jmp .lable84
.lable66:
	movsbq -49(%rbp), %rdx
	cmpq $100, %rdx
	sete %dl
	movzbq %dl, %rdx
	movq %rdx, %rbx
	testq %rbx, %rbx
	setz %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable68
	movq dir(%rip), %rbx
	cmpq $0, %rbx
	setne %bl
	movzbq %bl, %rbx
	movq %rbx, %rdx
	testq %rdx, %rdx
	setz %dl
	movzbq %dl, %rdx
	testq %rdx, %rdx
	jnz .lable68
	jmp .lable69
.lable68:
	movq $0, %rdx
	jmp .lable70
.lable69:
	movq $1, %rdx
.lable70:
	testq %rdx, %rdx
	jnz .lable71
	jmp .lable72
.lable71:
	movq $1, dir(%rip)
	jmp .lable84
.lable72:
	movsbq -49(%rbp), %rdx
	cmpq $119, %rdx
	sete %dl
	movzbq %dl, %rdx
	movq %rdx, %rbx
	testq %rbx, %rbx
	setz %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable74
	movq dir(%rip), %rbx
	cmpq $3, %rbx
	setne %bl
	movzbq %bl, %rbx
	movq %rbx, %rdx
	testq %rdx, %rdx
	setz %dl
	movzbq %dl, %rdx
	testq %rdx, %rdx
	jnz .lable74
	jmp .lable75
.lable74:
	movq $0, %rdx
	jmp .lable76
.lable75:
	movq $1, %rdx
.lable76:
	testq %rdx, %rdx
	jnz .lable77
	jmp .lable78
.lable77:
	movq $2, dir(%rip)
	jmp .lable84
.lable78:
	movsbq -49(%rbp), %rdx
	cmpq $115, %rdx
	sete %dl
	movzbq %dl, %rdx
	movq %rdx, %rbx
	testq %rbx, %rbx
	setz %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable80
	movq dir(%rip), %rbx
	cmpq $2, %rbx
	setne %bl
	movzbq %bl, %rbx
	movq %rbx, %rdx
	testq %rdx, %rdx
	setz %dl
	movzbq %dl, %rdx
	testq %rdx, %rdx
	jnz .lable80
	jmp .lable81
.lable80:
	movq $0, %rdx
	jmp .lable82
.lable81:
	movq $1, %rdx
.lable82:
	testq %rdx, %rdx
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
	movq exit(%rip), %rdx
	testq %rdx, %rdx
	jnz .lable86
.lable88:
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

