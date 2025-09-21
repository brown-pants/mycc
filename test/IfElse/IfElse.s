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
func:
	pushq %rbp
	movq %rsp, %rbp
	subq $504, %rsp
	movq 16(%rbp), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -8(%rbp)
	cmpq $0, -8(%rbp)
	jne .lable0
	jmp .lable1
.lable0:
	movq 24(%rbp), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -16(%rbp)
	cmpq $0, -16(%rbp)
	jne .lable2
	jmp .lable3
.lable2:
	pushq $'1'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -24(%rbp)
	pushq $'1'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -32(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -40(%rbp)
	jmp .lable4
.lable3:
	movq 24(%rbp), %rax
	movq $2, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -48(%rbp)
	cmpq $0, -48(%rbp)
	jne .lable5
	jmp .lable6
.lable5:
	pushq $'1'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -56(%rbp)
	pushq $'2'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -64(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -72(%rbp)
	jmp .lable7
.lable6:
	movq 24(%rbp), %rax
	movq $3, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -80(%rbp)
	cmpq $0, -80(%rbp)
	jne .lable8
	jmp .lable9
.lable8:
	pushq $'1'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -88(%rbp)
	pushq $'3'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -96(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -104(%rbp)
	jmp .lable10
.lable9:
	pushq $'1'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -112(%rbp)
	pushq $'$'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -120(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -128(%rbp)
.lable10:
.lable7:
.lable4:
	jmp .lable11
.lable1:
	movq 16(%rbp), %rax
	movq $2, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -136(%rbp)
	cmpq $0, -136(%rbp)
	jne .lable12
	jmp .lable13
.lable12:
	movq 24(%rbp), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -144(%rbp)
	cmpq $0, -144(%rbp)
	jne .lable14
	jmp .lable15
.lable14:
	pushq $'2'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -152(%rbp)
	pushq $'1'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -160(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -168(%rbp)
	jmp .lable16
.lable15:
	movq 24(%rbp), %rax
	movq $2, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -176(%rbp)
	cmpq $0, -176(%rbp)
	jne .lable17
	jmp .lable18
.lable17:
	pushq $'2'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -184(%rbp)
	pushq $'2'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -192(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -200(%rbp)
	jmp .lable19
.lable18:
	movq 24(%rbp), %rax
	movq $3, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -208(%rbp)
	cmpq $0, -208(%rbp)
	jne .lable20
	jmp .lable21
.lable20:
	pushq $'2'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -216(%rbp)
	pushq $'3'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -224(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -232(%rbp)
	jmp .lable22
.lable21:
	pushq $'2'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -240(%rbp)
	pushq $'$'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -248(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -256(%rbp)
.lable22:
.lable19:
.lable16:
	jmp .lable23
.lable13:
	movq 16(%rbp), %rax
	movq $3, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -264(%rbp)
	cmpq $0, -264(%rbp)
	jne .lable24
	jmp .lable25
.lable24:
	movq 24(%rbp), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -272(%rbp)
	cmpq $0, -272(%rbp)
	jne .lable26
	jmp .lable27
.lable26:
	pushq $'3'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -280(%rbp)
	pushq $'1'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -288(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -296(%rbp)
	jmp .lable28
.lable27:
	movq 24(%rbp), %rax
	movq $2, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -304(%rbp)
	cmpq $0, -304(%rbp)
	jne .lable29
	jmp .lable30
.lable29:
	pushq $'3'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -312(%rbp)
	pushq $'2'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -320(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -328(%rbp)
	jmp .lable31
.lable30:
	movq 24(%rbp), %rax
	movq $3, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -336(%rbp)
	cmpq $0, -336(%rbp)
	jne .lable32
	jmp .lable33
.lable32:
	pushq $'3'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -344(%rbp)
	pushq $'3'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -352(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -360(%rbp)
	jmp .lable34
.lable33:
	pushq $'3'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -368(%rbp)
	pushq $'$'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -376(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -384(%rbp)
.lable34:
.lable31:
.lable28:
	jmp .lable35
.lable25:
	movq 24(%rbp), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -392(%rbp)
	cmpq $0, -392(%rbp)
	jne .lable36
	jmp .lable37
.lable36:
	pushq $'$'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -400(%rbp)
	pushq $'1'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -408(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -416(%rbp)
	jmp .lable38
.lable37:
	movq 24(%rbp), %rax
	movq $2, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -424(%rbp)
	cmpq $0, -424(%rbp)
	jne .lable39
	jmp .lable40
.lable39:
	pushq $'$'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -432(%rbp)
	pushq $'2'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -440(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -448(%rbp)
	jmp .lable41
.lable40:
	movq 24(%rbp), %rax
	movq $3, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -456(%rbp)
	cmpq $0, -456(%rbp)
	jne .lable42
	jmp .lable43
.lable42:
	pushq $'$'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -464(%rbp)
	pushq $'3'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -472(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -480(%rbp)
	jmp .lable44
.lable43:
	pushq $'$'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -488(%rbp)
	pushq $'$'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -496(%rbp)
	pushq $'
'
	call mycc_putchar
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -504(%rbp)
.lable44:
.lable41:
.lable38:
.lable35:
.lable23:
.lable11:
func_exit:
	leave
	retq
.text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $64, %rsp
	movq $1, -8(%rbp)
.lable45:
	movq -8(%rbp), %rax
	movq $4, %rbx
	cmp %rbx, %rax
	setng %cl
	movzbq %cl, %rax
	movq %rax, -24(%rbp)
	cmpq $0, -24(%rbp)
	jne .lable46
	jmp .lable47
.lable46:
	movq $1, -16(%rbp)
.lable48:
	movq -16(%rbp), %rax
	movq $4, %rbx
	cmp %rbx, %rax
	setng %cl
	movzbq %cl, %rax
	movq %rax, -32(%rbp)
	cmpq $0, -32(%rbp)
	jne .lable49
	jmp .lable50
.lable49:
	pushq -16(%rbp)
	pushq -8(%rbp)
	call func
	addq $16, %rsp
	movq %rax, %rax
	movq %rax, -40(%rbp)
	movq -16(%rbp), %rax
	addq $1, %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	movq %rax, -16(%rbp)
	jmp .lable48
.lable50:
	movq -8(%rbp), %rax
	addq $1, %rax
	movq %rax, -56(%rbp)
	movq -56(%rbp), %rax
	movq %rax, -8(%rbp)
	jmp .lable45
.lable47:
	movq $0, %rax
	jmp main_exit
main_exit:
	leave
	retq
.global _start
_start:
	call main
	movq $60, %rax
	xorq %rdi, %rdi
	syscall

