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

.text
func:
	pushq %rbp
	movq %rsp, %rbp
	pushq 16(%rbp)
	pushq 24(%rbp)
	subq $8, %rsp
	movq -8(%rbp), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -24(%rbp)
	cmpq $0, -24(%rbp)
	jne .lable0
	jmp .lable1
.lable0:
	subq $8, %rsp
	movq -16(%rbp), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -32(%rbp)
	cmpq $0, -32(%rbp)
	jne .lable2
	jmp .lable3
.lable2:
	pushq $'1'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -40(%rbp)
	pushq $'1'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -48(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -56(%rbp)
	jmp .lable4
.lable3:
	subq $8, %rsp
	movq -16(%rbp), %rax
	movq $2, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -64(%rbp)
	cmpq $0, -64(%rbp)
	jne .lable5
	jmp .lable6
.lable5:
	pushq $'1'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -72(%rbp)
	pushq $'2'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -80(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -88(%rbp)
	jmp .lable7
.lable6:
	subq $8, %rsp
	movq -16(%rbp), %rax
	movq $3, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -96(%rbp)
	cmpq $0, -96(%rbp)
	jne .lable8
	jmp .lable9
.lable8:
	pushq $'1'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -104(%rbp)
	pushq $'3'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -112(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -120(%rbp)
	jmp .lable10
.lable9:
	pushq $'1'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -128(%rbp)
	pushq $'$'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -136(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -144(%rbp)
.lable10:
.lable7:
.lable4:
	jmp .lable11
.lable1:
	subq $8, %rsp
	movq -8(%rbp), %rax
	movq $2, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -152(%rbp)
	cmpq $0, -152(%rbp)
	jne .lable12
	jmp .lable13
.lable12:
	subq $8, %rsp
	movq -16(%rbp), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -160(%rbp)
	cmpq $0, -160(%rbp)
	jne .lable14
	jmp .lable15
.lable14:
	pushq $'2'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -168(%rbp)
	pushq $'1'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -176(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -184(%rbp)
	jmp .lable16
.lable15:
	subq $8, %rsp
	movq -16(%rbp), %rax
	movq $2, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -192(%rbp)
	cmpq $0, -192(%rbp)
	jne .lable17
	jmp .lable18
.lable17:
	pushq $'2'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -200(%rbp)
	pushq $'2'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -208(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -216(%rbp)
	jmp .lable19
.lable18:
	subq $8, %rsp
	movq -16(%rbp), %rax
	movq $3, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -224(%rbp)
	cmpq $0, -224(%rbp)
	jne .lable20
	jmp .lable21
.lable20:
	pushq $'2'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -232(%rbp)
	pushq $'3'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -240(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -248(%rbp)
	jmp .lable22
.lable21:
	pushq $'2'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -256(%rbp)
	pushq $'$'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -264(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -272(%rbp)
.lable22:
.lable19:
.lable16:
	jmp .lable23
.lable13:
	subq $8, %rsp
	movq -8(%rbp), %rax
	movq $3, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -280(%rbp)
	cmpq $0, -280(%rbp)
	jne .lable24
	jmp .lable25
.lable24:
	subq $8, %rsp
	movq -16(%rbp), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -288(%rbp)
	cmpq $0, -288(%rbp)
	jne .lable26
	jmp .lable27
.lable26:
	pushq $'3'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -296(%rbp)
	pushq $'1'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -304(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -312(%rbp)
	jmp .lable28
.lable27:
	subq $8, %rsp
	movq -16(%rbp), %rax
	movq $2, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -320(%rbp)
	cmpq $0, -320(%rbp)
	jne .lable29
	jmp .lable30
.lable29:
	pushq $'3'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -328(%rbp)
	pushq $'2'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -336(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -344(%rbp)
	jmp .lable31
.lable30:
	subq $8, %rsp
	movq -16(%rbp), %rax
	movq $3, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -352(%rbp)
	cmpq $0, -352(%rbp)
	jne .lable32
	jmp .lable33
.lable32:
	pushq $'3'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -360(%rbp)
	pushq $'3'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -368(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -376(%rbp)
	jmp .lable34
.lable33:
	pushq $'3'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -384(%rbp)
	pushq $'$'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -392(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -400(%rbp)
.lable34:
.lable31:
.lable28:
	jmp .lable35
.lable25:
	subq $8, %rsp
	movq -16(%rbp), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -408(%rbp)
	cmpq $0, -408(%rbp)
	jne .lable36
	jmp .lable37
.lable36:
	pushq $'$'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -416(%rbp)
	pushq $'1'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -424(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -432(%rbp)
	jmp .lable38
.lable37:
	subq $8, %rsp
	movq -16(%rbp), %rax
	movq $2, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -440(%rbp)
	cmpq $0, -440(%rbp)
	jne .lable39
	jmp .lable40
.lable39:
	pushq $'$'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -448(%rbp)
	pushq $'2'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -456(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -464(%rbp)
	jmp .lable41
.lable40:
	subq $8, %rsp
	movq -16(%rbp), %rax
	movq $3, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -472(%rbp)
	cmpq $0, -472(%rbp)
	jne .lable42
	jmp .lable43
.lable42:
	pushq $'$'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -480(%rbp)
	pushq $'3'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -488(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -496(%rbp)
	jmp .lable44
.lable43:
	pushq $'$'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -504(%rbp)
	pushq $'$'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -512(%rbp)
	pushq $'
'
	call mycc_putchar
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -520(%rbp)
.lable44:
.lable41:
.lable38:
.lable35:
.lable23:
.lable11:
func_exit:
	addq $520, %rsp
	leave
	retq
.text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	subq $8, %rsp
	movq $1, -8(%rbp)
.lable45:
	subq $8, %rsp
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
	subq $8, %rsp
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
	subq $8, %rsp
	movq %rax, %rax
	movq %rax, -40(%rbp)
	subq $8, %rsp
	movq -16(%rbp), %rax
	addq $1, %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	movq %rax, -16(%rbp)
	jmp .lable48
.lable50:
	subq $8, %rsp
	movq -8(%rbp), %rax
	addq $1, %rax
	movq %rax, -56(%rbp)
	movq -56(%rbp), %rax
	movq %rax, -8(%rbp)
	jmp .lable45
.lable47:
	subq $8, %rsp
	movq $0, %rax
	jmp main_exit
main_exit:
	addq $64, %rsp
	leave
	retq
.global _start
_start:
	call main
	movq $60, %rax
	xorq %rdi, %rdi
	syscall

