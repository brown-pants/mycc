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
	.global arr
	.bss
arr:
	.zero 13
	.text
puts:
	pushq %rbp
	movq %rsp, %rbp
	subq $18, %rsp
.lable0:
	movq 16(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -9(%rbp)
	movsbq -9(%rbp), %rax
	movq $0, %rbx
	cmp %rbx, %rax
	setne %cl
	movzbq %cl, %rax
	movq %rax, -8(%rbp)
	cmpq $0, -8(%rbp)
	jne .lable1
	jmp .lable2
.lable1:
	movq 16(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -10(%rbp)
	movsbq -10(%rbp), %rax
	pushq %rax
	call mycc_putchar
	addq $8, %rsp
	movq 16(%rbp), %rax
	addq $1, %rax
	movq %rax, -18(%rbp)
	movq -18(%rbp), %rax
	movq %rax, 16(%rbp)
	jmp .lable0
.lable2:
puts_exit:
	leave
	retq
	.text
HelloWorld_1:
	pushq %rbp
	movq %rsp, %rbp
	subq $21, %rsp
	movq $72, %rax
	movq $0, %rdx
	leaq -13(%rbp), %rbx
	movb %al, (%rbx, %rdx)
	movq $101, %rax
	movq $1, %rdx
	leaq -13(%rbp), %rbx
	movb %al, (%rbx, %rdx)
	movq $108, %rax
	movq $2, %rdx
	leaq -13(%rbp), %rbx
	movb %al, (%rbx, %rdx)
	movq $108, %rax
	movq $3, %rdx
	leaq -13(%rbp), %rbx
	movb %al, (%rbx, %rdx)
	movq $111, %rax
	movq $4, %rdx
	leaq -13(%rbp), %rbx
	movb %al, (%rbx, %rdx)
	movq $32, %rax
	movq $5, %rdx
	leaq -13(%rbp), %rbx
	movb %al, (%rbx, %rdx)
	movq $87, %rax
	movq $6, %rdx
	leaq -13(%rbp), %rbx
	movb %al, (%rbx, %rdx)
	movq $111, %rax
	movq $7, %rdx
	leaq -13(%rbp), %rbx
	movb %al, (%rbx, %rdx)
	movq $114, %rax
	movq $8, %rdx
	leaq -13(%rbp), %rbx
	movb %al, (%rbx, %rdx)
	movq $108, %rax
	movq $9, %rdx
	leaq -13(%rbp), %rbx
	movb %al, (%rbx, %rdx)
	movq $100, %rax
	movq $10, %rdx
	leaq -13(%rbp), %rbx
	movb %al, (%rbx, %rdx)
	movq $10, %rax
	movq $11, %rdx
	leaq -13(%rbp), %rbx
	movb %al, (%rbx, %rdx)
	movq $0, %rax
	movq $12, %rdx
	leaq -13(%rbp), %rbx
	movb %al, (%rbx, %rdx)
	leaq -13(%rbp), %rax
	movq %rax, -21(%rbp)
	pushq -21(%rbp)
	call puts
	addq $8, %rsp
HelloWorld_1_exit:
	leave
	retq
	.text
HelloWorld_2:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	movq $72, %rax
	movq $0, %rdx
	leaq arr(%rip), %rbx
	movb %al, (%rbx, %rdx)
	movq $101, %rax
	movq $1, %rdx
	leaq arr(%rip), %rbx
	movb %al, (%rbx, %rdx)
	movq $108, %rax
	movq $2, %rdx
	leaq arr(%rip), %rbx
	movb %al, (%rbx, %rdx)
	movq $108, %rax
	movq $3, %rdx
	leaq arr(%rip), %rbx
	movb %al, (%rbx, %rdx)
	movq $111, %rax
	movq $4, %rdx
	leaq arr(%rip), %rbx
	movb %al, (%rbx, %rdx)
	movq $32, %rax
	movq $5, %rdx
	leaq arr(%rip), %rbx
	movb %al, (%rbx, %rdx)
	movq $87, %rax
	movq $6, %rdx
	leaq arr(%rip), %rbx
	movb %al, (%rbx, %rdx)
	movq $111, %rax
	movq $7, %rdx
	leaq arr(%rip), %rbx
	movb %al, (%rbx, %rdx)
	movq $114, %rax
	movq $8, %rdx
	leaq arr(%rip), %rbx
	movb %al, (%rbx, %rdx)
	movq $108, %rax
	movq $9, %rdx
	leaq arr(%rip), %rbx
	movb %al, (%rbx, %rdx)
	movq $100, %rax
	movq $10, %rdx
	leaq arr(%rip), %rbx
	movb %al, (%rbx, %rdx)
	movq $10, %rax
	movq $11, %rdx
	leaq arr(%rip), %rbx
	movb %al, (%rbx, %rdx)
	movq $0, %rax
	movq $12, %rdx
	leaq arr(%rip), %rbx
	movb %al, (%rbx, %rdx)
	leaq arr(%rip), %rax
	movq %rax, -8(%rbp)
	pushq -8(%rbp)
	call puts
	addq $8, %rsp
HelloWorld_2_exit:
	leave
	retq
	.text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $0, %rsp
	call HelloWorld_1
	addq $0, %rsp
	call HelloWorld_2
	addq $0, %rsp
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

