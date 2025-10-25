	.global printPositiveInt
	.text
printPositiveInt:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	movq 16(%rbp), %rdi
	movq %rdi, -8(%rbp)
	movq -8(%rbp), %rdi
	testq %rdi, %rdi
	jnz .lable0
	jmp .lable1
.lable0:
	movq $10, %rdi
	pushq %rax
	movq -8(%rbp), %rax
	cqto
	idivq %rdi
	movq %rax, %rax
	addq $8, %rsp
	pushq %rax
	pushq %rax
	call printPositiveInt
	addq $8, %rsp
	popq %rax
	movq $10, %rdi
	pushq %rax
	movq -8(%rbp), %rax
	cqto
	idivq %rdi
	movq %rdx, %rax
	addq $8, %rsp
	movq %rax, %rbx
	addq $48, %rbx
	pushq %rbx
	call putchar
	addq $8, %rsp
.lable1:
	leave
	retq
	.global printSignInt
	.text
printSignInt:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	movq 16(%rbp), %rbx
	movq %rbx, -8(%rbp)
	movq -8(%rbp), %rbx
	cmpq $0, %rbx
	setl %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable2
	jmp .lable3
.lable2:
	pushq $45
	call putchar
	addq $8, %rsp
	movq -8(%rbp), %rbx
	negq %rbx
	movq %rbx, -8(%rbp)
.lable3:
	movq -8(%rbp), %rbx
	cmpq $0, %rbx
	sete %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable4
	jmp .lable5
.lable4:
	pushq $48
	call putchar
	addq $8, %rsp
	jmp .lable6
.lable5:
	pushq -8(%rbp)
	call printPositiveInt
	addq $8, %rsp
.lable6:
	pushq $10
	call putchar
	addq $8, %rsp
	leave
	retq
	.global factorial
	.text
factorial:
	pushq %rbp
	movq %rsp, %rbp
	subq $24, %rsp
	movq 16(%rbp), %rbx
	movq %rbx, -8(%rbp)
	movq $1, -16(%rbp)
	movq $2, -24(%rbp)
.lable7:
	movq -24(%rbp), %rbx
	cmpq -8(%rbp), %rbx
	setng %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable10
	jmp .lable8
.lable10:
	movq -16(%rbp), %rbx
	imulq -24(%rbp), %rbx
	movq %rbx, -16(%rbp)
	movq -24(%rbp), %rbx
	addq $1, %rbx
	movq %rbx, -24(%rbp)
	jmp .lable7
.lable8:
	movq -16(%rbp), %rax
	leave
	retq
	.global factorial_recursion
	.text
factorial_recursion:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq 16(%rbp), %rbx
	movq %rbx, -8(%rbp)
	movq -8(%rbp), %rbx
	cmpq $1, %rbx
	sete %bl
	movzbq %bl, %rbx
	testq %rbx, %rbx
	jnz .lable11
	jmp .lable12
.lable11:
	movq $1, %rax
	jmp factorial_recursion_exit
.lable12:
	movq -8(%rbp), %rbx
	subq $1, %rbx
	pushq %rbx
	call factorial_recursion
	addq $8, %rsp
	movq %rax, %rbx
	movq -8(%rbp), %rax
	imulq %rbx, %rax
	movq %rax, %rax
factorial_recursion_exit:
	leave
	retq
	.global fib
	.text
fib:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	movq 16(%rbp), %rax
	movq %rax, -8(%rbp)
	movq -8(%rbp), %rax
	cmpq $1, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable13
	movq -8(%rbp), %rax
	cmpq $2, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable13
	jmp .lable14
.lable13:
	movq $1, %rax
	jmp .lable15
.lable14:
	movq $0, %rax
.lable15:
	testq %rax, %rax
	jnz .lable16
	jmp .lable17
.lable16:
	movq $1, %rax
	jmp fib_exit
.lable17:
	movq -8(%rbp), %rax
	subq $1, %rax
	pushq %rax
	call fib
	addq $8, %rsp
	movq %rax, %rax
	movq -8(%rbp), %rbx
	subq $2, %rbx
	pushq %rax
	pushq %rbx
	call fib
	addq $8, %rsp
	movq %rax, %rbx
	popq %rax
	movq %rax, %rcx
	addq %rbx, %rcx
	movq %rcx, %rax
fib_exit:
	leave
	retq
	.global main
	.text
main:
	pushq %rbp
	movq %rsp, %rbp
	pushq $5
	call factorial
	addq $8, %rsp
	movq %rax, %rcx
	pushq %rcx
	call printSignInt
	addq $8, %rsp
	pushq $5
	call factorial
	addq $8, %rsp
	movq %rax, %rcx
	movq %rcx, %rbx
	negq %rbx
	pushq %rbx
	call printSignInt
	addq $8, %rsp
	pushq $10
	call factorial_recursion
	addq $8, %rsp
	movq %rax, %rbx
	pushq %rbx
	call printSignInt
	addq $8, %rsp
	pushq $10
	call factorial_recursion
	addq $8, %rsp
	movq %rax, %rbx
	movq %rbx, %rcx
	negq %rcx
	pushq %rcx
	call printSignInt
	addq $8, %rsp
	pushq $5
	call fib
	addq $8, %rsp
	movq %rax, %rcx
	pushq %rcx
	call printSignInt
	addq $8, %rsp
	pushq $8
	call fib
	addq $8, %rsp
	movq %rax, %rcx
	pushq %rcx
	call printSignInt
	addq $8, %rsp
	leave
	retq
	.global _start
	.text
_start:
	call main
	movq $60, %rax
	xorq %rdi, %rdi
	syscall

