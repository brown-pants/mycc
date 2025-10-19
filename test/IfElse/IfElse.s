	.data
.str15:
	.string "n == ? m == ?"
	.data
.str14:
	.string "n == ? m == 3"
	.data
.str13:
	.string "n == ? m == 2"
	.data
.str12:
	.string "n == ? m == 1"
	.data
.str11:
	.string "n == 3 m == ?"
	.data
.str10:
	.string "n == 3 m == 3"
	.data
.str9:
	.string "n == 3 m == 2"
	.data
.str8:
	.string "n == 3 m == 1"
	.data
.str7:
	.string "n == 2 m == ?"
	.data
.str6:
	.string "n == 2 m == 3"
	.data
.str5:
	.string "n == 2 m == 2"
	.data
.str4:
	.string "n == 2 m == 1"
	.data
.str3:
	.string "n == 1 m == ?"
	.data
.str2:
	.string "n == 1 m == 3"
	.data
.str1:
	.string "n == 1 m == 2"
	.data
.str0:
	.string "n == 1 m == 1"
	.global func
	.text
func:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq 16(%rbp), %rdi
	movq %rdi, -8(%rbp)
	movq 24(%rbp), %rdi
	movq %rdi, -16(%rbp)
	movq -8(%rbp), %rax
	cmpq $1, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable0
	jmp .lable1
.lable0:
	movq -16(%rbp), %rax
	cmpq $1, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable2
	jmp .lable3
.lable2:
	leaq .str0(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
	jmp .lable4
.lable3:
	movq -16(%rbp), %rax
	cmpq $2, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable5
	jmp .lable6
.lable5:
	leaq .str1(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
	jmp .lable7
.lable6:
	movq -16(%rbp), %rax
	cmpq $3, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable8
	jmp .lable9
.lable8:
	leaq .str2(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
	jmp .lable10
.lable9:
	leaq .str3(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
.lable10:
.lable7:
.lable4:
	jmp .lable11
.lable1:
	movq -8(%rbp), %rax
	cmpq $2, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable12
	jmp .lable13
.lable12:
	movq -16(%rbp), %rax
	cmpq $1, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable14
	jmp .lable15
.lable14:
	leaq .str4(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
	jmp .lable16
.lable15:
	movq -16(%rbp), %rax
	cmpq $2, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable17
	jmp .lable18
.lable17:
	leaq .str5(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
	jmp .lable19
.lable18:
	movq -16(%rbp), %rax
	cmpq $3, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable20
	jmp .lable21
.lable20:
	leaq .str6(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
	jmp .lable22
.lable21:
	leaq .str7(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
.lable22:
.lable19:
.lable16:
	jmp .lable23
.lable13:
	movq -8(%rbp), %rax
	cmpq $3, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable24
	jmp .lable25
.lable24:
	movq -16(%rbp), %rax
	cmpq $1, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable26
	jmp .lable27
.lable26:
	leaq .str8(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
	jmp .lable28
.lable27:
	movq -16(%rbp), %rax
	cmpq $2, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable29
	jmp .lable30
.lable29:
	leaq .str9(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
	jmp .lable31
.lable30:
	movq -16(%rbp), %rax
	cmpq $3, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable32
	jmp .lable33
.lable32:
	leaq .str10(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
	jmp .lable34
.lable33:
	leaq .str11(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
.lable34:
.lable31:
.lable28:
	jmp .lable35
.lable25:
	movq -16(%rbp), %rax
	cmpq $1, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable36
	jmp .lable37
.lable36:
	leaq .str12(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
	jmp .lable38
.lable37:
	movq -16(%rbp), %rax
	cmpq $2, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable39
	jmp .lable40
.lable39:
	leaq .str13(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
	jmp .lable41
.lable40:
	movq -16(%rbp), %rax
	cmpq $3, %rax
	sete %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable42
	jmp .lable43
.lable42:
	leaq .str14(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
	jmp .lable44
.lable43:
	leaq .str15(%rip), %rax
	pushq %rax
	call puts
	addq $8, %rsp
.lable44:
.lable41:
.lable38:
.lable35:
.lable23:
.lable11:
func_exit:
	leave
	retq
	.global main
	.text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq $1, -8(%rbp)
.lable45:
	movq -8(%rbp), %rax
	cmpq $4, %rax
	setng %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable48
	jmp .lable46
.lable48:
	movq $1, -16(%rbp)
.lable49:
	movq -16(%rbp), %rax
	cmpq $4, %rax
	setng %al
	movzbq %al, %rax
	testq %rax, %rax
	jnz .lable52
	jmp .lable50
.lable52:
	pushq -16(%rbp)
	pushq -8(%rbp)
	call func
	addq $16, %rsp
.lable51:
	movq -16(%rbp), %rax
	addq $1, %rax
	movq %rax, -16(%rbp)
	jmp .lable49
.lable50:
.lable47:
	movq -8(%rbp), %rax
	addq $1, %rax
	movq %rax, -8(%rbp)
	jmp .lable45
.lable46:
	movq $0, %rax
	jmp main_exit
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

