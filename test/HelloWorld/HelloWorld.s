	.section .rodata
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
	leaq -13(%rbp), %rax
	movq $72, %rdi
	movb %dil, (%rax)
	movq %rax, %rbx
	addq $1, %rbx
	movq $101, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $2, %rbx
	movq $108, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $3, %rbx
	movq $108, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $4, %rbx
	movq $111, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $5, %rbx
	movq $32, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $6, %rbx
	movq $87, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $7, %rbx
	movq $111, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $8, %rbx
	movq $114, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $9, %rbx
	movq $108, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $10, %rbx
	movq $100, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $11, %rbx
	movq $0, %rdi
	movb %dil, (%rbx)
	pushq %rax
	pushq %rax
	call puts
	addq $8, %rsp
	popq %rax
	leave
	retq
	.global HelloWorld_2
	.text
HelloWorld_2:
	pushq %rbp
	movq %rsp, %rbp
	leaq arr(%rip), %rax
	movq $72, %rbx
	movb %bl, (%rax)
	movq %rax, %rbx
	addq $1, %rbx
	movq $101, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $2, %rbx
	movq $108, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $3, %rbx
	movq $108, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $4, %rbx
	movq $111, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $5, %rbx
	movq $32, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $6, %rbx
	movq $87, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $7, %rbx
	movq $111, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $8, %rbx
	movq $114, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $9, %rbx
	movq $108, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $10, %rbx
	movq $100, %rdi
	movb %dil, (%rbx)
	movq %rax, %rbx
	addq $11, %rbx
	movq $0, %rdi
	movb %dil, (%rbx)
	pushq %rax
	call puts
	addq $8, %rsp
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
	pushq %rax
	call puts
	addq $8, %rsp
	popq %rax
	movq %rax, -8(%rbp)
	pushq S(%rip)
	call puts
	addq $8, %rsp
	pushq -8(%rbp)
	call puts
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

