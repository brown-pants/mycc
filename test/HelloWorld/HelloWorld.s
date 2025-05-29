.text
mycc_putchar:
	pushq %rbp
	movq %rsp, %rbp
	pushq 16(%rbp)
	movq $1, %rax
	movq $1, %rdi
	leaq -8(%rbp), %rsi
	movq $1, %rdx
	syscall
	addq $8, %rsp
	leave
	retq

	.global arr
	.bss
arr:
	.zero 96
.text
HelloWorld_1:
	pushq %rbp
	movq %rsp, %rbp
	subq $96, %rsp
	subq $8, %rsp
	movq $0, -104(%rbp)
	movq -104(%rbp), %rdx
	leaq -96(%rbp), %rcx
	movq $'H', (%rdx, %rcx)
	subq $8, %rsp
	movq $8, -112(%rbp)
	movq -112(%rbp), %rdx
	leaq -96(%rbp), %rcx
	movq $'e', (%rdx, %rcx)
	subq $8, %rsp
	movq $16, -120(%rbp)
	movq -120(%rbp), %rdx
	leaq -96(%rbp), %rcx
	movq $'l', (%rdx, %rcx)
	subq $8, %rsp
	movq $24, -128(%rbp)
	movq -128(%rbp), %rdx
	leaq -96(%rbp), %rcx
	movq $'l', (%rdx, %rcx)
	subq $8, %rsp
	movq $32, -136(%rbp)
	movq -136(%rbp), %rdx
	leaq -96(%rbp), %rcx
	movq $'o', (%rdx, %rcx)
	subq $8, %rsp
	movq $40, -144(%rbp)
	movq -144(%rbp), %rdx
	leaq -96(%rbp), %rcx
	movq $' ', (%rdx, %rcx)
	subq $8, %rsp
	movq $48, -152(%rbp)
	movq -152(%rbp), %rdx
	leaq -96(%rbp), %rcx
	movq $'W', (%rdx, %rcx)
	subq $8, %rsp
	movq $56, -160(%rbp)
	movq -160(%rbp), %rdx
	leaq -96(%rbp), %rcx
	movq $'o', (%rdx, %rcx)
	subq $8, %rsp
	movq $64, -168(%rbp)
	movq -168(%rbp), %rdx
	leaq -96(%rbp), %rcx
	movq $'r', (%rdx, %rcx)
	subq $8, %rsp
	movq $72, -176(%rbp)
	movq -176(%rbp), %rdx
	leaq -96(%rbp), %rcx
	movq $'l', (%rdx, %rcx)
	subq $8, %rsp
	movq $80, -184(%rbp)
	movq -184(%rbp), %rdx
	leaq -96(%rbp), %rcx
	movq $'d', (%rdx, %rcx)
	subq $8, %rsp
	movq $88, -192(%rbp)
	movq -192(%rbp), %rdx
	leaq -96(%rbp), %rcx
	movq $'
', (%rdx, %rcx)
	subq $8, %rsp
	movq $0, -200(%rbp)
.lable0:
	subq $8, %rsp
	movq -200(%rbp), %rax
	movq $12, %rbx
	cmp %rbx, %rax
	setl %cl
	movzbq %cl, %rax
	movq %rax, -208(%rbp)
	cmpq $0, -208(%rbp)
	jne .lable1
	jmp .lable2
.lable1:
	subq $8, %rsp
	movq -200(%rbp), %rax
	imulq $8, %rax
	movq %rax, -216(%rbp)
	movq -216(%rbp), %rdx
	leaq -96(%rbp), %rcx
	subq $8, %rsp
	movq (%rdx, %rcx), %rbx
	movq %rbx, -224(%rbp)
	pushq -224(%rbp)
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -232(%rbp)
	subq $8, %rsp
	movq -200(%rbp), %rax
	addq $1, %rax
	movq %rax, -240(%rbp)
	movq -240(%rbp), %rax
	movq %rax, -200(%rbp)
	jmp .lable0
.lable2:
HelloWorld_1_exit:
	addq $240, %rsp
	leave
	retq
.text
HelloWorld_2:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	movq $0, -8(%rbp)
	movq -8(%rbp), %rdx
	leaq arr(%rip), %rcx
	movq $'H', (%rdx, %rcx)
	subq $8, %rsp
	movq $8, -16(%rbp)
	movq -16(%rbp), %rdx
	leaq arr(%rip), %rcx
	movq $'e', (%rdx, %rcx)
	subq $8, %rsp
	movq $16, -24(%rbp)
	movq -24(%rbp), %rdx
	leaq arr(%rip), %rcx
	movq $'l', (%rdx, %rcx)
	subq $8, %rsp
	movq $24, -32(%rbp)
	movq -32(%rbp), %rdx
	leaq arr(%rip), %rcx
	movq $'l', (%rdx, %rcx)
	subq $8, %rsp
	movq $32, -40(%rbp)
	movq -40(%rbp), %rdx
	leaq arr(%rip), %rcx
	movq $'o', (%rdx, %rcx)
	subq $8, %rsp
	movq $40, -48(%rbp)
	movq -48(%rbp), %rdx
	leaq arr(%rip), %rcx
	movq $' ', (%rdx, %rcx)
	subq $8, %rsp
	movq $48, -56(%rbp)
	movq -56(%rbp), %rdx
	leaq arr(%rip), %rcx
	movq $'W', (%rdx, %rcx)
	subq $8, %rsp
	movq $56, -64(%rbp)
	movq -64(%rbp), %rdx
	leaq arr(%rip), %rcx
	movq $'o', (%rdx, %rcx)
	subq $8, %rsp
	movq $64, -72(%rbp)
	movq -72(%rbp), %rdx
	leaq arr(%rip), %rcx
	movq $'r', (%rdx, %rcx)
	subq $8, %rsp
	movq $72, -80(%rbp)
	movq -80(%rbp), %rdx
	leaq arr(%rip), %rcx
	movq $'l', (%rdx, %rcx)
	subq $8, %rsp
	movq $80, -88(%rbp)
	movq -88(%rbp), %rdx
	leaq arr(%rip), %rcx
	movq $'d', (%rdx, %rcx)
	subq $8, %rsp
	movq $88, -96(%rbp)
	movq -96(%rbp), %rdx
	leaq arr(%rip), %rcx
	movq $'
', (%rdx, %rcx)
	subq $8, %rsp
	movq $0, -104(%rbp)
.lable3:
	subq $8, %rsp
	movq -104(%rbp), %rax
	movq $12, %rbx
	cmp %rbx, %rax
	setl %cl
	movzbq %cl, %rax
	movq %rax, -112(%rbp)
	cmpq $0, -112(%rbp)
	jne .lable4
	jmp .lable5
.lable4:
	subq $8, %rsp
	movq -104(%rbp), %rax
	imulq $8, %rax
	movq %rax, -120(%rbp)
	movq -120(%rbp), %rdx
	leaq arr(%rip), %rcx
	subq $8, %rsp
	movq (%rdx, %rcx), %rbx
	movq %rbx, -128(%rbp)
	pushq -128(%rbp)
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -136(%rbp)
	subq $8, %rsp
	movq -104(%rbp), %rax
	addq $1, %rax
	movq %rax, -144(%rbp)
	movq -144(%rbp), %rax
	movq %rax, -104(%rbp)
	jmp .lable3
.lable5:
HelloWorld_2_exit:
	addq $144, %rsp
	leave
	retq
.text
main:
	pushq %rbp
	movq %rsp, %rbp
	call HelloWorld_1
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -8(%rbp)
	call HelloWorld_2
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -16(%rbp)
main_exit:
	addq $16, %rsp
	leave
	retq
.global _start
_start:
	call main
	movq $60, %rax
	xorq %rdi, %rdi
	syscall

