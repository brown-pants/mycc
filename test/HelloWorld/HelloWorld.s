	.data
.str0:
	.string "Hello World"
	.global arr
	.bss
arr:
	.zero 12
	.global HelloWorld_1
	.text
HelloWorld_1:
	pushq %rbp
	movq %rsp, %rbp
	subq $13, %rsp
	movq $72, %rdi
	movb %dil, -13(%rbp)
	leaq -13(%rbp), %rax
	movq %rax, %rbx
	addq $1, %rbx
	movq $101, %rdi
	movb %dil, (%rbx)
	leaq -13(%rbp), %rbx
	movq %rbx, %rax
	addq $2, %rax
	movq $108, %rbx
	movb %bl, (%rax)
	leaq -13(%rbp), %rax
	movq %rax, %rbx
	addq $3, %rbx
	movq $108, %rax
	movb %al, (%rbx)
	leaq -13(%rbp), %rbx
	movq %rbx, %rax
	addq $4, %rax
	movq $111, %rbx
	movb %bl, (%rax)
	leaq -13(%rbp), %rax
	movq %rax, %rbx
	addq $5, %rbx
	movq $32, %rax
	movb %al, (%rbx)
	leaq -13(%rbp), %rbx
	movq %rbx, %rax
	addq $6, %rax
	movq $87, %rbx
	movb %bl, (%rax)
	leaq -13(%rbp), %rax
	movq %rax, %rbx
	addq $7, %rbx
	movq $111, %rax
	movb %al, (%rbx)
	leaq -13(%rbp), %rbx
	movq %rbx, %rax
	addq $8, %rax
	movq $114, %rbx
	movb %bl, (%rax)
	leaq -13(%rbp), %rax
	movq %rax, %rbx
	addq $9, %rbx
	movq $108, %rax
	movb %al, (%rbx)
	leaq -13(%rbp), %rbx
	movq %rbx, %rax
	addq $10, %rax
	movq $100, %rbx
	movb %bl, (%rax)
	leaq -13(%rbp), %rax
	movq %rax, %rbx
	addq $11, %rbx
	movq $0, %rax
	movb %al, (%rbx)
	leaq -13(%rbp), %rbx
	pushq %rbx
	call puts
	addq $8, %rsp
HelloWorld_1_exit:
	leave
	retq
	.global HelloWorld_2
	.text
HelloWorld_2:
	pushq %rbp
	movq %rsp, %rbp
	movq $72, %rbx
	movb %bl, arr(%rip)
	leaq arr(%rip), %rbx
	movq %rbx, %rax
	addq $1, %rax
	movq $101, %rbx
	movb %bl, (%rax)
	leaq arr(%rip), %rax
	movq %rax, %rbx
	addq $2, %rbx
	movq $108, %rax
	movb %al, (%rbx)
	leaq arr(%rip), %rbx
	movq %rbx, %rax
	addq $3, %rax
	movq $108, %rbx
	movb %bl, (%rax)
	leaq arr(%rip), %rax
	movq %rax, %rbx
	addq $4, %rbx
	movq $111, %rax
	movb %al, (%rbx)
	leaq arr(%rip), %rbx
	movq %rbx, %rax
	addq $5, %rax
	movq $32, %rbx
	movb %bl, (%rax)
	leaq arr(%rip), %rax
	movq %rax, %rbx
	addq $6, %rbx
	movq $87, %rax
	movb %al, (%rbx)
	leaq arr(%rip), %rbx
	movq %rbx, %rax
	addq $7, %rax
	movq $111, %rbx
	movb %bl, (%rax)
	leaq arr(%rip), %rax
	movq %rax, %rbx
	addq $8, %rbx
	movq $114, %rax
	movb %al, (%rbx)
	leaq arr(%rip), %rbx
	movq %rbx, %rax
	addq $9, %rax
	movq $108, %rbx
	movb %bl, (%rax)
	leaq arr(%rip), %rax
	movq %rax, %rbx
	addq $10, %rbx
	movq $100, %rax
	movb %al, (%rbx)
	leaq arr(%rip), %rbx
	movq %rbx, %rax
	addq $11, %rax
	movq $0, %rbx
	movb %bl, (%rax)
	leaq arr(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
HelloWorld_2_exit:
	leave
	retq
	.global S
	.data
S:
	.quad .str0
	.global main
	.text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	call HelloWorld_1
	call HelloWorld_2
	leaq .str0(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
	leaq .str0(%rip), %rax
	movq %rax, -8(%rbp)
	pushq S(%rip)
	call puts
	addq $8, %rsp
	pushq -8(%rbp)
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

