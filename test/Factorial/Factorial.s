	.global printInt
	.text
printInt:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movq 16(%rbp), %rax
	movq %rax, -8(%rbp)
	cmpq $0, -8(%rbp)
	jne .lable0
	jmp .lable1
.lable0:
	movq -8(%rbp), %rax
	cqto
	movq $10, %rcx
	idivq %rcx
	movq %rax, -16(%rbp)
	pushq -16(%rbp)
	call printInt
	addq $8, %rsp
	movq -8(%rbp), %rax
	cqto
	movq $10, %rcx
	idivq %rcx
	movq %rdx, -24(%rbp)
	movq -24(%rbp), %rax
	addq $48, %rax
	movq %rax, -32(%rbp)
	pushq -32(%rbp)
	call putchar
	addq $8, %rsp
.lable1:
printInt_exit:
	leave
	retq
	.global printSignInt
	.text
printSignInt:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movq 16(%rbp), %rax
	movq %rax, -8(%rbp)
	movq -8(%rbp), %rax
	movq $0, %rbx
	cmp %rbx, %rax
	setl %cl
	movzbq %cl, %rax
	movq %rax, -16(%rbp)
	cmpq $0, -16(%rbp)
	jne .lable2
	jmp .lable3
.lable2:
	pushq $45
	call putchar
	addq $8, %rsp
	movq -8(%rbp), %rax
	negq %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	movq %rax, -8(%rbp)
.lable3:
	movq -8(%rbp), %rax
	movq $0, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -32(%rbp)
	cmpq $0, -32(%rbp)
	jne .lable4
	jmp .lable5
.lable4:
	pushq $48
	call putchar
	addq $8, %rsp
	jmp .lable6
.lable5:
	pushq -8(%rbp)
	call printInt
	addq $8, %rsp
.lable6:
	pushq $10
	call putchar
	addq $8, %rsp
printSignInt_exit:
	leave
	retq
	.global factorial
	.text
factorial:
	pushq %rbp
	movq %rsp, %rbp
	subq $48, %rsp
	movq 16(%rbp), %rax
	movq %rax, -8(%rbp)
	movq $1, -16(%rbp)
	movq $2, -24(%rbp)
.lable7:
	movq -24(%rbp), %rax
	movq -8(%rbp), %rbx
	cmp %rbx, %rax
	setng %cl
	movzbq %cl, %rax
	movq %rax, -32(%rbp)
	cmpq $0, -32(%rbp)
	jne .lable10
	jmp .lable8
.lable10:
	movq -16(%rbp), %rax
	imulq -24(%rbp), %rax
	movq %rax, -40(%rbp)
	movq -40(%rbp), %rax
	movq %rax, -16(%rbp)
.lable9:
	movq -24(%rbp), %rax
	addq $1, %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	movq %rax, -24(%rbp)
	jmp .lable7
.lable8:
	movq -16(%rbp), %rax
	movq %rax, %rax
	jmp factorial_exit
factorial_exit:
	leave
	retq
	.global factorial_recursion
	.text
factorial_recursion:
	pushq %rbp
	movq %rsp, %rbp
	subq $48, %rsp
	movq 16(%rbp), %rax
	movq %rax, -8(%rbp)
	movq -8(%rbp), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -16(%rbp)
	cmpq $0, -16(%rbp)
	jne .lable11
	jmp .lable12
.lable11:
	movq $1, %rax
	jmp factorial_recursion_exit
.lable12:
	movq -8(%rbp), %rax
	subq $1, %rax
	movq %rax, -32(%rbp)
	pushq -32(%rbp)
	call factorial_recursion
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -40(%rbp)
	movq -8(%rbp), %rax
	imulq -40(%rbp), %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	movq %rax, %rax
	jmp factorial_recursion_exit
factorial_recursion_exit:
	leave
	retq
	.global main
	.text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $56, %rsp
	pushq -8(%rbp)
	call printSignInt
	addq $8, %rsp
	pushq $5
	call factorial
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -16(%rbp)
	pushq -16(%rbp)
	call printSignInt
	addq $8, %rsp
	pushq $5
	call factorial
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	negq %rax
	movq %rax, -32(%rbp)
	pushq -32(%rbp)
	call printSignInt
	addq $8, %rsp
	pushq $10
	call factorial_recursion
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -40(%rbp)
	pushq -40(%rbp)
	call printSignInt
	addq $8, %rsp
	pushq $10
	call factorial_recursion
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	negq %rax
	movq %rax, -56(%rbp)
	pushq -56(%rbp)
	call printSignInt
	addq $8, %rsp
main_exit:
	leave
	retq
	.global _start
	.text
_start:
	call main
	movq $60, %rax
	xorq %rdi, %rdi
	syscall

