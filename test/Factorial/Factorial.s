.text
mycc_putchar:
	pushq %rbp
	movq %rsp, %rbp
	movq $1, %rax
	movq $1, %rdi
	leaq 16(%rbp), %rsi
	movq $1, %rdx
	syscall
	leave
	retq

.text
mod:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movq 16(%rbp), %rax
	cqto
	movq $10, %rcx
	divq %rcx
	movq %rax, -8(%rbp)
	movq -8(%rbp), %rax
	imulq $10, %rax
	movq %rax, -16(%rbp)
	movq 16(%rbp), %rax
	subq -16(%rbp), %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	movq %rax, %rax
	jmp mod_exit
mod_exit:
	leave
	retq
.text
printInt:
	pushq %rbp
	movq %rsp, %rbp
	subq $40, %rsp
	cmpq $0, 16(%rbp)
	jne .lable0
	jmp .lable1
.lable0:
	movq 16(%rbp), %rax
	cqto
	movq $10, %rcx
	divq %rcx
	movq %rax, -8(%rbp)
	pushq -8(%rbp)
	call printInt
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -16(%rbp)
	pushq 16(%rbp)
	call mod
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	addq $48, %rax
	movq %rax, -32(%rbp)
	pushq -32(%rbp)
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -40(%rbp)
.lable1:
printInt_exit:
	leave
	retq
.text
printSignInt:
	pushq %rbp
	movq %rsp, %rbp
	subq $56, %rsp
	movq 16(%rbp), %rax
	movq $0, %rbx
	cmp %rbx, %rax
	setl %cl
	movzbq %cl, %rax
	movq %rax, -8(%rbp)
	cmpq $0, -8(%rbp)
	jne .lable2
	jmp .lable3
.lable2:
	pushq $'-'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -16(%rbp)
	movq $0, %rax
	subq 16(%rbp), %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	movq %rax, 16(%rbp)
.lable3:
	movq 16(%rbp), %rax
	movq $0, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -32(%rbp)
	cmpq $0, -32(%rbp)
	jne .lable4
	jmp .lable5
.lable4:
	pushq $'0'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -40(%rbp)
	jmp .lable6
.lable5:
	pushq 16(%rbp)
	call printInt
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -48(%rbp)
.lable6:
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -56(%rbp)
printSignInt_exit:
	leave
	retq
.text
factorial:
	pushq %rbp
	movq %rsp, %rbp
	subq $48, %rsp
	movq $1, -8(%rbp)
	movq $2, -16(%rbp)
.lable7:
	movq -16(%rbp), %rax
	movq 16(%rbp), %rbx
	cmp %rbx, %rax
	setng %cl
	movzbq %cl, %rax
	movq %rax, -24(%rbp)
	cmpq $0, -24(%rbp)
	jne .lable8
	jmp .lable9
.lable8:
	movq -8(%rbp), %rax
	imulq -16(%rbp), %rax
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rax
	movq %rax, -8(%rbp)
	movq -16(%rbp), %rax
	addq $1, %rax
	movq %rax, -40(%rbp)
	movq -40(%rbp), %rax
	movq %rax, -16(%rbp)
	jmp .lable7
.lable9:
	movq -8(%rbp), %rax
	movq %rax, %rax
	jmp factorial_exit
factorial_exit:
	leave
	retq
.text
factorial_recursion:
	pushq %rbp
	movq %rsp, %rbp
	subq $48, %rsp
	movq 16(%rbp), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -8(%rbp)
	cmpq $0, -8(%rbp)
	jne .lable10
	jmp .lable11
.lable10:
	movq $1, %rax
	jmp factorial_recursion_exit
.lable11:
	movq 16(%rbp), %rax
	subq $1, %rax
	movq %rax, -32(%rbp)
	pushq -32(%rbp)
	call factorial_recursion
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -40(%rbp)
	movq 16(%rbp), %rax
	imulq -40(%rbp), %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	movq %rax, %rax
	jmp factorial_recursion_exit
factorial_recursion_exit:
	leave
	retq
.text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $96, %rsp
	pushq -8(%rbp)
	call printSignInt
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -16(%rbp)
	pushq $5
	call factorial
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -24(%rbp)
	pushq -24(%rbp)
	call printSignInt
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -32(%rbp)
	pushq $5
	call factorial
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -40(%rbp)
	movq $0, %rax
	subq -40(%rbp), %rax
	movq %rax, -48(%rbp)
	pushq -48(%rbp)
	call printSignInt
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -56(%rbp)
	pushq $10
	call factorial_recursion
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -64(%rbp)
	pushq -64(%rbp)
	call printSignInt
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -72(%rbp)
	pushq $10
	call factorial_recursion
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -80(%rbp)
	movq $0, %rax
	subq -80(%rbp), %rax
	movq %rax, -88(%rbp)
	pushq -88(%rbp)
	call printSignInt
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -96(%rbp)
main_exit:
	leave
	retq
.global _start
_start:
	call main
	movq $60, %rax
	xorq %rdi, %rdi
	syscall

