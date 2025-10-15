	.data
.str0:
	.string "TERM=xterm clear"
	.global new_food
	.text
new_food:
	pushq %rbp
	movq %rsp, %rbp
	subq $169, %rsp
	call rand
	addq $0, %rsp
	movq %rax, %rax
	movq %rax, -8(%rbp)
	movq -8(%rbp), %rax
	addq $1, %rax
	movq %rax, -16(%rbp)
	movq Width(%rip), %rax
	subq $1, %rax
	movq %rax, -24(%rbp)
	movq -16(%rbp), %rax
	cqto
	movq -24(%rbp), %rcx
	idivq %rcx
	movq %rdx, -32(%rbp)
	movq -32(%rbp), %rax
	movq %rax, -40(%rbp)
	call rand
	addq $0, %rsp
	movq %rax, %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	addq $1, %rax
	movq %rax, -56(%rbp)
	movq Height(%rip), %rax
	subq $1, %rax
	movq %rax, -64(%rbp)
	movq -56(%rbp), %rax
	cqto
	movq -64(%rbp), %rcx
	idivq %rcx
	movq %rdx, -72(%rbp)
	movq -72(%rbp), %rax
	movq %rax, -80(%rbp)
	movq -80(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, -88(%rbp)
	movq -88(%rbp), %rax
	addq -40(%rbp), %rax
	movq %rax, -96(%rbp)
	movq -96(%rbp), %rax
	imulq $1, %rax
	movq %rax, -104(%rbp)
	leaq Map(%rip), %rax
	movq %rax, -120(%rbp)
	movq -120(%rbp), %rax
	addq -104(%rbp), %rax
	movq %rax, -112(%rbp)
	movq -112(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -129(%rbp)
	movsbq -129(%rbp), %rax
	movq $32, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -128(%rbp)
	cmpq $0, -128(%rbp)
	jne .lable0
	jmp .lable1
.lable0:
	movq -80(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, -137(%rbp)
	movq -137(%rbp), %rax
	addq -40(%rbp), %rax
	movq %rax, -145(%rbp)
	movq -145(%rbp), %rax
	imulq $1, %rax
	movq %rax, -153(%rbp)
	leaq Map(%rip), %rax
	movq %rax, -169(%rbp)
	movq -169(%rbp), %rax
	addq -153(%rbp), %rax
	movq %rax, -161(%rbp)
	movq $42, %rax
	movq -161(%rbp), %rbx
	movb %al, (%rbx)
	jmp .lable2
.lable1:
	call new_food
	addq $0, %rsp
.lable2:
new_food_exit:
	leave
	retq
	.global init
	.text
init:
	pushq %rbp
	movq %rsp, %rbp
	subq $448, %rsp
	movq $0, -8(%rbp)
.lable3:
	movq -8(%rbp), %rax
	movq snake_len(%rip), %rbx
	cmp %rbx, %rax
	setl %cl
	movzbq %cl, %rax
	movq %rax, -24(%rbp)
	cmpq $0, -24(%rbp)
	jne .lable6
	jmp .lable4
.lable6:
	movq Width(%rip), %rax
	cqto
	movq $2, %rcx
	idivq %rcx
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rax
	addq -8(%rbp), %rax
	movq %rax, -40(%rbp)
	movq -8(%rbp), %rax
	imulq $8, %rax
	movq %rax, -48(%rbp)
	leaq snake_x(%rip), %rax
	movq %rax, -64(%rbp)
	movq -64(%rbp), %rax
	addq -48(%rbp), %rax
	movq %rax, -56(%rbp)
	movq -40(%rbp), %rax
	movq -56(%rbp), %rbx
	movq %rax, (%rbx)
	movq Height(%rip), %rax
	cqto
	movq $2, %rcx
	idivq %rcx
	movq %rax, -72(%rbp)
	movq -8(%rbp), %rax
	imulq $8, %rax
	movq %rax, -80(%rbp)
	leaq snake_y(%rip), %rax
	movq %rax, -96(%rbp)
	movq -96(%rbp), %rax
	addq -80(%rbp), %rax
	movq %rax, -88(%rbp)
	movq -72(%rbp), %rax
	movq -88(%rbp), %rbx
	movq %rax, (%rbx)
.lable5:
	movq -8(%rbp), %rax
	addq $1, %rax
	movq %rax, -104(%rbp)
	movq -104(%rbp), %rax
	movq %rax, -8(%rbp)
	jmp .lable3
.lable4:
	movq $0, -8(%rbp)
.lable7:
	movq -8(%rbp), %rax
	movq Height(%rip), %rbx
	cmp %rbx, %rax
	setl %cl
	movzbq %cl, %rax
	movq %rax, -112(%rbp)
	cmpq $0, -112(%rbp)
	jne .lable10
	jmp .lable8
.lable10:
	movq $0, -16(%rbp)
.lable11:
	movq -16(%rbp), %rax
	movq Width(%rip), %rbx
	cmp %rbx, %rax
	setl %cl
	movzbq %cl, %rax
	movq %rax, -120(%rbp)
	cmpq $0, -120(%rbp)
	jne .lable14
	jmp .lable12
.lable14:
	movq -8(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, -128(%rbp)
	movq -128(%rbp), %rax
	addq -16(%rbp), %rax
	movq %rax, -136(%rbp)
	movq -136(%rbp), %rax
	imulq $1, %rax
	movq %rax, -144(%rbp)
	leaq Map(%rip), %rax
	movq %rax, -160(%rbp)
	movq -160(%rbp), %rax
	addq -144(%rbp), %rax
	movq %rax, -152(%rbp)
	movq -152(%rbp), %rax
	movq %rax, -168(%rbp)
	movq -16(%rbp), %rax
	movq $0, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -176(%rbp)
	cmpq $0, -176(%rbp)
	jne .lable15
	movq Width(%rip), %rax
	subq $1, %rax
	movq %rax, -184(%rbp)
	movq -16(%rbp), %rax
	movq -184(%rbp), %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -192(%rbp)
	cmpq $0, -192(%rbp)
	jne .lable15
	movq -8(%rbp), %rax
	movq $0, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -200(%rbp)
	cmpq $0, -200(%rbp)
	jne .lable15
	movq Height(%rip), %rax
	subq $1, %rax
	movq %rax, -208(%rbp)
	movq -8(%rbp), %rax
	movq -208(%rbp), %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -216(%rbp)
	cmpq $0, -216(%rbp)
	jne .lable15
	jmp .lable16
.lable15:
	movq $1, -224(%rbp)
	jmp .lable17
.lable16:
	movq $0, -224(%rbp)
.lable17:
	cmpq $0, -224(%rbp)
	jne .lable18
	jmp .lable19
.lable18:
	movq $35, %rax
	movq -168(%rbp), %rbx
	movb %al, (%rbx)
	jmp .lable20
.lable19:
	movq $32, %rax
	movq -168(%rbp), %rbx
	movb %al, (%rbx)
.lable20:
.lable13:
	movq -16(%rbp), %rax
	addq $1, %rax
	movq %rax, -240(%rbp)
	movq -240(%rbp), %rax
	movq %rax, -16(%rbp)
	jmp .lable11
.lable12:
.lable9:
	movq -8(%rbp), %rax
	addq $1, %rax
	movq %rax, -248(%rbp)
	movq -248(%rbp), %rax
	movq %rax, -8(%rbp)
	jmp .lable7
.lable8:
	movq $0, -8(%rbp)
.lable21:
	movq -8(%rbp), %rax
	movq snake_len(%rip), %rbx
	cmp %rbx, %rax
	setl %cl
	movzbq %cl, %rax
	movq %rax, -256(%rbp)
	cmpq $0, -256(%rbp)
	jne .lable24
	jmp .lable22
.lable24:
	movq -8(%rbp), %rax
	imulq $8, %rax
	movq %rax, -264(%rbp)
	leaq snake_x(%rip), %rax
	movq %rax, -280(%rbp)
	movq -280(%rbp), %rax
	addq -264(%rbp), %rax
	movq %rax, -272(%rbp)
	movq -272(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -296(%rbp)
	movq -296(%rbp), %rax
	movq %rax, -288(%rbp)
	movq -8(%rbp), %rax
	imulq $8, %rax
	movq %rax, -304(%rbp)
	leaq snake_y(%rip), %rax
	movq %rax, -320(%rbp)
	movq -320(%rbp), %rax
	addq -304(%rbp), %rax
	movq %rax, -312(%rbp)
	movq -312(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -336(%rbp)
	movq -336(%rbp), %rax
	movq %rax, -328(%rbp)
	movq -8(%rbp), %rax
	movq $0, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -344(%rbp)
	cmpq $0, -344(%rbp)
	jne .lable25
	jmp .lable26
.lable25:
	movq -328(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, -352(%rbp)
	movq -352(%rbp), %rax
	addq -288(%rbp), %rax
	movq %rax, -360(%rbp)
	movq -360(%rbp), %rax
	imulq $1, %rax
	movq %rax, -368(%rbp)
	leaq Map(%rip), %rax
	movq %rax, -384(%rbp)
	movq -384(%rbp), %rax
	addq -368(%rbp), %rax
	movq %rax, -376(%rbp)
	movq $64, %rax
	movq -376(%rbp), %rbx
	movb %al, (%rbx)
	jmp .lable27
.lable26:
	movq -328(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, -392(%rbp)
	movq -392(%rbp), %rax
	addq -288(%rbp), %rax
	movq %rax, -400(%rbp)
	movq -400(%rbp), %rax
	imulq $1, %rax
	movq %rax, -408(%rbp)
	leaq Map(%rip), %rax
	movq %rax, -424(%rbp)
	movq -424(%rbp), %rax
	addq -408(%rbp), %rax
	movq %rax, -416(%rbp)
	movq $43, %rax
	movq -416(%rbp), %rbx
	movb %al, (%rbx)
.lable27:
.lable23:
	movq -8(%rbp), %rax
	addq $1, %rax
	movq %rax, -432(%rbp)
	movq -432(%rbp), %rax
	movq %rax, -8(%rbp)
	jmp .lable21
.lable22:
	pushq $0
	call time
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -440(%rbp)
	pushq -440(%rbp)
	call srand
	addq $8, %rsp
	movq %rax, %rax
	movq %rax, -448(%rbp)
	call new_food
	addq $0, %rsp
init_exit:
	leave
	retq
	.global draw
	.text
draw:
	pushq %rbp
	movq %rsp, %rbp
	subq $105, %rsp
	leaq .str0(%rip), %rax
	movq %rax, -8(%rbp)
	pushq -8(%rbp)
	call system
	addq $8, %rsp
	movq $0, -16(%rbp)
.lable28:
	movq -16(%rbp), %rax
	movq Height(%rip), %rbx
	cmp %rbx, %rax
	setl %cl
	movzbq %cl, %rax
	movq %rax, -32(%rbp)
	cmpq $0, -32(%rbp)
	jne .lable31
	jmp .lable29
.lable31:
	movq $0, -24(%rbp)
.lable32:
	movq -24(%rbp), %rax
	movq Width(%rip), %rbx
	cmp %rbx, %rax
	setl %cl
	movzbq %cl, %rax
	movq %rax, -40(%rbp)
	cmpq $0, -40(%rbp)
	jne .lable35
	jmp .lable33
.lable35:
	movq -16(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	addq -24(%rbp), %rax
	movq %rax, -56(%rbp)
	movq -56(%rbp), %rax
	imulq $1, %rax
	movq %rax, -64(%rbp)
	leaq Map(%rip), %rax
	movq %rax, -80(%rbp)
	movq -80(%rbp), %rax
	addq -64(%rbp), %rax
	movq %rax, -72(%rbp)
	movq -72(%rbp), %rax
	movq %rax, -88(%rbp)
	movq -88(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -89(%rbp)
	movsbq -89(%rbp), %rax
	pushq %rax
	call putchar
	addq $8, %rsp
.lable34:
	movq -24(%rbp), %rax
	addq $1, %rax
	movq %rax, -97(%rbp)
	movq -97(%rbp), %rax
	movq %rax, -24(%rbp)
	jmp .lable32
.lable33:
	pushq $10
	call putchar
	addq $8, %rsp
.lable30:
	movq -16(%rbp), %rax
	addq $1, %rax
	movq %rax, -105(%rbp)
	movq -105(%rbp), %rax
	movq %rax, -16(%rbp)
	jmp .lable28
.lable29:
draw_exit:
	leave
	retq
	.global update
	.text
update:
	pushq %rbp
	movq %rsp, %rbp
	subq $660, %rsp
	movq snake_len(%rip), %rax
	subq $1, %rax
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	imulq $8, %rax
	movq %rax, -24(%rbp)
	leaq snake_x(%rip), %rax
	movq %rax, -40(%rbp)
	movq -40(%rbp), %rax
	addq -24(%rbp), %rax
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -56(%rbp)
	movq -56(%rbp), %rax
	movq %rax, -48(%rbp)
	movq snake_len(%rip), %rax
	subq $1, %rax
	movq %rax, -64(%rbp)
	movq -64(%rbp), %rax
	imulq $8, %rax
	movq %rax, -72(%rbp)
	leaq snake_y(%rip), %rax
	movq %rax, -88(%rbp)
	movq -88(%rbp), %rax
	addq -72(%rbp), %rax
	movq %rax, -80(%rbp)
	movq -80(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -104(%rbp)
	movq -104(%rbp), %rax
	movq %rax, -96(%rbp)
	movq snake_x(%rip), %rax
	movq %rax, -112(%rbp)
	movq snake_y(%rip), %rax
	movq %rax, -120(%rbp)
	cmpq $0, grown(%rip)
	jne .lable36
	jmp .lable37
.lable36:
	movq $0, grown(%rip)
	movq snake_len(%rip), %rax
	addq $1, %rax
	movq %rax, -128(%rbp)
	movq -128(%rbp), %rax
	movq %rax, snake_len(%rip)
	jmp .lable38
.lable37:
	movq -96(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, -136(%rbp)
	movq -136(%rbp), %rax
	addq -48(%rbp), %rax
	movq %rax, -144(%rbp)
	movq -144(%rbp), %rax
	imulq $1, %rax
	movq %rax, -152(%rbp)
	leaq Map(%rip), %rax
	movq %rax, -168(%rbp)
	movq -168(%rbp), %rax
	addq -152(%rbp), %rax
	movq %rax, -160(%rbp)
	movq $32, %rax
	movq -160(%rbp), %rbx
	movb %al, (%rbx)
.lable38:
	movq -120(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, -176(%rbp)
	movq -176(%rbp), %rax
	addq -112(%rbp), %rax
	movq %rax, -184(%rbp)
	movq -184(%rbp), %rax
	imulq $1, %rax
	movq %rax, -192(%rbp)
	leaq Map(%rip), %rax
	movq %rax, -208(%rbp)
	movq -208(%rbp), %rax
	addq -192(%rbp), %rax
	movq %rax, -200(%rbp)
	movq $43, %rax
	movq -200(%rbp), %rbx
	movb %al, (%rbx)
	movq snake_len(%rip), %rax
	subq $1, %rax
	movq %rax, -216(%rbp)
	movq -216(%rbp), %rax
	movq %rax, -8(%rbp)
.lable39:
	movq -8(%rbp), %rax
	movq $0, %rbx
	cmp %rbx, %rax
	setg %cl
	movzbq %cl, %rax
	movq %rax, -224(%rbp)
	cmpq $0, -224(%rbp)
	jne .lable42
	jmp .lable40
.lable42:
	movq -8(%rbp), %rax
	subq $1, %rax
	movq %rax, -232(%rbp)
	movq -232(%rbp), %rax
	imulq $8, %rax
	movq %rax, -240(%rbp)
	leaq snake_x(%rip), %rax
	movq %rax, -256(%rbp)
	movq -256(%rbp), %rax
	addq -240(%rbp), %rax
	movq %rax, -248(%rbp)
	movq -8(%rbp), %rax
	imulq $8, %rax
	movq %rax, -264(%rbp)
	leaq snake_x(%rip), %rax
	movq %rax, -280(%rbp)
	movq -280(%rbp), %rax
	addq -264(%rbp), %rax
	movq %rax, -272(%rbp)
	movq -248(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -288(%rbp)
	movq -288(%rbp), %rax
	movq -272(%rbp), %rbx
	movq %rax, (%rbx)
	movq -8(%rbp), %rax
	subq $1, %rax
	movq %rax, -296(%rbp)
	movq -296(%rbp), %rax
	imulq $8, %rax
	movq %rax, -304(%rbp)
	leaq snake_y(%rip), %rax
	movq %rax, -320(%rbp)
	movq -320(%rbp), %rax
	addq -304(%rbp), %rax
	movq %rax, -312(%rbp)
	movq -8(%rbp), %rax
	imulq $8, %rax
	movq %rax, -328(%rbp)
	leaq snake_y(%rip), %rax
	movq %rax, -344(%rbp)
	movq -344(%rbp), %rax
	addq -328(%rbp), %rax
	movq %rax, -336(%rbp)
	movq -312(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -352(%rbp)
	movq -352(%rbp), %rax
	movq -336(%rbp), %rbx
	movq %rax, (%rbx)
.lable41:
	movq -8(%rbp), %rax
	subq $1, %rax
	movq %rax, -360(%rbp)
	movq -360(%rbp), %rax
	movq %rax, -8(%rbp)
	jmp .lable39
.lable40:
	movq dir(%rip), %rax
	movq $0, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -368(%rbp)
	cmpq $0, -368(%rbp)
	jne .lable43
	jmp .lable44
.lable43:
	movq snake_x(%rip), %rax
	subq $1, %rax
	movq %rax, -376(%rbp)
	movq -376(%rbp), %rax
	movq %rax, snake_x(%rip)
	jmp .lable45
.lable44:
	movq dir(%rip), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -384(%rbp)
	cmpq $0, -384(%rbp)
	jne .lable46
	jmp .lable47
.lable46:
	movq snake_x(%rip), %rax
	addq $1, %rax
	movq %rax, -392(%rbp)
	movq -392(%rbp), %rax
	movq %rax, snake_x(%rip)
	jmp .lable48
.lable47:
	movq dir(%rip), %rax
	movq $2, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -400(%rbp)
	cmpq $0, -400(%rbp)
	jne .lable49
	jmp .lable50
.lable49:
	movq snake_y(%rip), %rax
	subq $1, %rax
	movq %rax, -408(%rbp)
	movq -408(%rbp), %rax
	movq %rax, snake_y(%rip)
	jmp .lable51
.lable50:
	movq snake_y(%rip), %rax
	addq $1, %rax
	movq %rax, -416(%rbp)
	movq -416(%rbp), %rax
	movq %rax, snake_y(%rip)
.lable51:
.lable48:
.lable45:
	movq snake_x(%rip), %rax
	movq %rax, -112(%rbp)
	movq snake_y(%rip), %rax
	movq %rax, -120(%rbp)
	movq -120(%rbp), %rax
	imulq Width(%rip), %rax
	movq %rax, -424(%rbp)
	movq -424(%rbp), %rax
	addq -112(%rbp), %rax
	movq %rax, -432(%rbp)
	movq -432(%rbp), %rax
	imulq $1, %rax
	movq %rax, -440(%rbp)
	leaq Map(%rip), %rax
	movq %rax, -456(%rbp)
	movq -456(%rbp), %rax
	addq -440(%rbp), %rax
	movq %rax, -448(%rbp)
	movq -448(%rbp), %rax
	movq %rax, -464(%rbp)
	movq -464(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -473(%rbp)
	movsbq -473(%rbp), %rax
	movq $35, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -472(%rbp)
	cmpq $0, -472(%rbp)
	jne .lable52
	movq -464(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -482(%rbp)
	movsbq -482(%rbp), %rax
	movq $43, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -481(%rbp)
	cmpq $0, -481(%rbp)
	jne .lable52
	jmp .lable53
.lable52:
	movq $1, -490(%rbp)
	jmp .lable54
.lable53:
	movq $0, -490(%rbp)
.lable54:
	cmpq $0, -490(%rbp)
	jne .lable55
	jmp .lable56
.lable55:
	movq $1, exit(%rip)
	jmp update_exit
	jmp .lable57
.lable56:
	movq -464(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -507(%rbp)
	movsbq -507(%rbp), %rax
	movq $42, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -506(%rbp)
	cmpq $0, -506(%rbp)
	jne .lable58
	jmp .lable59
.lable58:
	movq $1, grown(%rip)
	call new_food
	addq $0, %rsp
.lable59:
.lable57:
	movq $64, %rax
	movq -464(%rbp), %rbx
	movb %al, (%rbx)
	call kbhit
	addq $0, %rsp
	movq %rax, %rax
	movq %rax, -515(%rbp)
	cmpq $0, -515(%rbp)
	jne .lable60
	jmp .lable61
.lable60:
	call getch
	addq $0, %rsp
	movq %rax, %rax
	movq %rax, -523(%rbp)
	movq -523(%rbp), %rax
	movb %al, -524(%rbp)
	movsbq -524(%rbp), %rax
	movq $97, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -532(%rbp)
	movq -532(%rbp), %rax
	testq %rax, %rax
	setz %al
	movb %al, -533(%rbp)
	cmpb $0, -533(%rbp)
	jne .lable62
	movq dir(%rip), %rax
	movq $1, %rbx
	cmp %rbx, %rax
	setne %cl
	movzbq %cl, %rax
	movq %rax, -541(%rbp)
	movq -541(%rbp), %rax
	testq %rax, %rax
	setz %al
	movb %al, -542(%rbp)
	cmpb $0, -542(%rbp)
	jne .lable62
	jmp .lable63
.lable62:
	movq $0, -550(%rbp)
	jmp .lable64
.lable63:
	movq $1, -550(%rbp)
.lable64:
	cmpq $0, -550(%rbp)
	jne .lable65
	jmp .lable66
.lable65:
	movq $0, dir(%rip)
	jmp .lable67
.lable66:
	movsbq -524(%rbp), %rax
	movq $100, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -566(%rbp)
	movq -566(%rbp), %rax
	testq %rax, %rax
	setz %al
	movb %al, -567(%rbp)
	cmpb $0, -567(%rbp)
	jne .lable68
	movq dir(%rip), %rax
	movq $0, %rbx
	cmp %rbx, %rax
	setne %cl
	movzbq %cl, %rax
	movq %rax, -575(%rbp)
	movq -575(%rbp), %rax
	testq %rax, %rax
	setz %al
	movb %al, -576(%rbp)
	cmpb $0, -576(%rbp)
	jne .lable68
	jmp .lable69
.lable68:
	movq $0, -584(%rbp)
	jmp .lable70
.lable69:
	movq $1, -584(%rbp)
.lable70:
	cmpq $0, -584(%rbp)
	jne .lable71
	jmp .lable72
.lable71:
	movq $1, dir(%rip)
	jmp .lable73
.lable72:
	movsbq -524(%rbp), %rax
	movq $119, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -600(%rbp)
	movq -600(%rbp), %rax
	testq %rax, %rax
	setz %al
	movb %al, -601(%rbp)
	cmpb $0, -601(%rbp)
	jne .lable74
	movq dir(%rip), %rax
	movq $3, %rbx
	cmp %rbx, %rax
	setne %cl
	movzbq %cl, %rax
	movq %rax, -609(%rbp)
	movq -609(%rbp), %rax
	testq %rax, %rax
	setz %al
	movb %al, -610(%rbp)
	cmpb $0, -610(%rbp)
	jne .lable74
	jmp .lable75
.lable74:
	movq $0, -618(%rbp)
	jmp .lable76
.lable75:
	movq $1, -618(%rbp)
.lable76:
	cmpq $0, -618(%rbp)
	jne .lable77
	jmp .lable78
.lable77:
	movq $2, dir(%rip)
	jmp .lable79
.lable78:
	movsbq -524(%rbp), %rax
	movq $115, %rbx
	cmp %rbx, %rax
	sete %cl
	movzbq %cl, %rax
	movq %rax, -634(%rbp)
	movq -634(%rbp), %rax
	testq %rax, %rax
	setz %al
	movb %al, -635(%rbp)
	cmpb $0, -635(%rbp)
	jne .lable80
	movq dir(%rip), %rax
	movq $2, %rbx
	cmp %rbx, %rax
	setne %cl
	movzbq %cl, %rax
	movq %rax, -643(%rbp)
	movq -643(%rbp), %rax
	testq %rax, %rax
	setz %al
	movb %al, -644(%rbp)
	cmpb $0, -644(%rbp)
	jne .lable80
	jmp .lable81
.lable80:
	movq $0, -652(%rbp)
	jmp .lable82
.lable81:
	movq $1, -652(%rbp)
.lable82:
	cmpq $0, -652(%rbp)
	jne .lable83
	jmp .lable84
.lable83:
	movq $3, dir(%rip)
.lable84:
.lable79:
.lable73:
.lable67:
.lable61:
update_exit:
	leave
	retq
	.global main
	.text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $0, %rsp
	call init
	addq $0, %rsp
.lable85:
	cmpq $0, exit(%rip)
	jne .lable87
	jmp .lable88
.lable87:
	jmp .lable86
.lable88:
	call draw
	addq $0, %rsp
	call update
	addq $0, %rsp
	pushq $100000
	call usleep
	addq $8, %rsp
	jmp .lable85
.lable86:
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

