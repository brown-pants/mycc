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
	.data
.str0:
	.string "Hello World\n"
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
	subq $213, %rsp
	movq $72, %rax
	movb %al, -13(%rbp)
	leaq -13(%rbp), %rax
	movq %rax, -29(%rbp)
	movq -29(%rbp), %rax
	addq $1, %rax
	movq %rax, -21(%rbp)
	movq $101, %rax
	movq -21(%rbp), %rbx
	movb %al, (%rbx)
	leaq -13(%rbp), %rax
	movq %rax, -45(%rbp)
	movq -45(%rbp), %rax
	addq $2, %rax
	movq %rax, -37(%rbp)
	movq $108, %rax
	movq -37(%rbp), %rbx
	movb %al, (%rbx)
	leaq -13(%rbp), %rax
	movq %rax, -61(%rbp)
	movq -61(%rbp), %rax
	addq $3, %rax
	movq %rax, -53(%rbp)
	movq $108, %rax
	movq -53(%rbp), %rbx
	movb %al, (%rbx)
	leaq -13(%rbp), %rax
	movq %rax, -77(%rbp)
	movq -77(%rbp), %rax
	addq $4, %rax
	movq %rax, -69(%rbp)
	movq $111, %rax
	movq -69(%rbp), %rbx
	movb %al, (%rbx)
	leaq -13(%rbp), %rax
	movq %rax, -93(%rbp)
	movq -93(%rbp), %rax
	addq $5, %rax
	movq %rax, -85(%rbp)
	movq $32, %rax
	movq -85(%rbp), %rbx
	movb %al, (%rbx)
	leaq -13(%rbp), %rax
	movq %rax, -109(%rbp)
	movq -109(%rbp), %rax
	addq $6, %rax
	movq %rax, -101(%rbp)
	movq $87, %rax
	movq -101(%rbp), %rbx
	movb %al, (%rbx)
	leaq -13(%rbp), %rax
	movq %rax, -125(%rbp)
	movq -125(%rbp), %rax
	addq $7, %rax
	movq %rax, -117(%rbp)
	movq $111, %rax
	movq -117(%rbp), %rbx
	movb %al, (%rbx)
	leaq -13(%rbp), %rax
	movq %rax, -141(%rbp)
	movq -141(%rbp), %rax
	addq $8, %rax
	movq %rax, -133(%rbp)
	movq $114, %rax
	movq -133(%rbp), %rbx
	movb %al, (%rbx)
	leaq -13(%rbp), %rax
	movq %rax, -157(%rbp)
	movq -157(%rbp), %rax
	addq $9, %rax
	movq %rax, -149(%rbp)
	movq $108, %rax
	movq -149(%rbp), %rbx
	movb %al, (%rbx)
	leaq -13(%rbp), %rax
	movq %rax, -173(%rbp)
	movq -173(%rbp), %rax
	addq $10, %rax
	movq %rax, -165(%rbp)
	movq $100, %rax
	movq -165(%rbp), %rbx
	movb %al, (%rbx)
	leaq -13(%rbp), %rax
	movq %rax, -189(%rbp)
	movq -189(%rbp), %rax
	addq $11, %rax
	movq %rax, -181(%rbp)
	movq $10, %rax
	movq -181(%rbp), %rbx
	movb %al, (%rbx)
	leaq -13(%rbp), %rax
	movq %rax, -205(%rbp)
	movq -205(%rbp), %rax
	addq $12, %rax
	movq %rax, -197(%rbp)
	movq $0, %rax
	movq -197(%rbp), %rbx
	movb %al, (%rbx)
	leaq -13(%rbp), %rax
	movq %rax, -213(%rbp)
	pushq -213(%rbp)
	call puts
	addq $8, %rsp
HelloWorld_1_exit:
	leave
	retq
	.text
HelloWorld_2:
	pushq %rbp
	movq %rsp, %rbp
	subq $200, %rsp
	movq $72, %rax
	movb %al, arr(%rip)
	leaq arr(%rip), %rax
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	addq $1, %rax
	movq %rax, -8(%rbp)
	movq $101, %rax
	movq -8(%rbp), %rbx
	movb %al, (%rbx)
	leaq arr(%rip), %rax
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rax
	addq $2, %rax
	movq %rax, -24(%rbp)
	movq $108, %rax
	movq -24(%rbp), %rbx
	movb %al, (%rbx)
	leaq arr(%rip), %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	addq $3, %rax
	movq %rax, -40(%rbp)
	movq $108, %rax
	movq -40(%rbp), %rbx
	movb %al, (%rbx)
	leaq arr(%rip), %rax
	movq %rax, -64(%rbp)
	movq -64(%rbp), %rax
	addq $4, %rax
	movq %rax, -56(%rbp)
	movq $111, %rax
	movq -56(%rbp), %rbx
	movb %al, (%rbx)
	leaq arr(%rip), %rax
	movq %rax, -80(%rbp)
	movq -80(%rbp), %rax
	addq $5, %rax
	movq %rax, -72(%rbp)
	movq $32, %rax
	movq -72(%rbp), %rbx
	movb %al, (%rbx)
	leaq arr(%rip), %rax
	movq %rax, -96(%rbp)
	movq -96(%rbp), %rax
	addq $6, %rax
	movq %rax, -88(%rbp)
	movq $87, %rax
	movq -88(%rbp), %rbx
	movb %al, (%rbx)
	leaq arr(%rip), %rax
	movq %rax, -112(%rbp)
	movq -112(%rbp), %rax
	addq $7, %rax
	movq %rax, -104(%rbp)
	movq $111, %rax
	movq -104(%rbp), %rbx
	movb %al, (%rbx)
	leaq arr(%rip), %rax
	movq %rax, -128(%rbp)
	movq -128(%rbp), %rax
	addq $8, %rax
	movq %rax, -120(%rbp)
	movq $114, %rax
	movq -120(%rbp), %rbx
	movb %al, (%rbx)
	leaq arr(%rip), %rax
	movq %rax, -144(%rbp)
	movq -144(%rbp), %rax
	addq $9, %rax
	movq %rax, -136(%rbp)
	movq $108, %rax
	movq -136(%rbp), %rbx
	movb %al, (%rbx)
	leaq arr(%rip), %rax
	movq %rax, -160(%rbp)
	movq -160(%rbp), %rax
	addq $10, %rax
	movq %rax, -152(%rbp)
	movq $100, %rax
	movq -152(%rbp), %rbx
	movb %al, (%rbx)
	leaq arr(%rip), %rax
	movq %rax, -176(%rbp)
	movq -176(%rbp), %rax
	addq $11, %rax
	movq %rax, -168(%rbp)
	movq $10, %rax
	movq -168(%rbp), %rbx
	movb %al, (%rbx)
	leaq arr(%rip), %rax
	movq %rax, -192(%rbp)
	movq -192(%rbp), %rax
	addq $12, %rax
	movq %rax, -184(%rbp)
	movq $0, %rax
	movq -184(%rbp), %rbx
	movb %al, (%rbx)
	leaq arr(%rip), %rax
	movq %rax, -200(%rbp)
	pushq -200(%rbp)
	call puts
	addq $8, %rsp
HelloWorld_2_exit:
	leave
	retq
	.data
S:
	.quad .str0
	.text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $24, %rsp
	call HelloWorld_1
	addq $0, %rsp
	call HelloWorld_2
	addq $0, %rsp
	leaq .str0(%rip), %rax
	movq %rax, -8(%rbp)
	pushq -8(%rbp)
	call puts
	addq $8, %rsp
	leaq .str0(%rip), %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	movq %rax, -16(%rbp)
	pushq S(%rip)
	call puts
	addq $8, %rsp
	pushq -16(%rbp)
	call puts
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

