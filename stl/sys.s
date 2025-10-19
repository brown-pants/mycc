	.text
	.global putchar
putchar:
	pushq %rbp
	movq %rsp, %rbp
    subq $1, %rsp
    movb 16(%rbp), %al
    movb %al, -1(%rbp)
	movq $1, %rax
	movq $1, %rdi
	leaq -1(%rbp), %rsi
	movq $1, %rdx
	syscall
	leave
	retq

.data
newline:
    .byte 10

.text
.globl puts
puts:
    pushq %rbp
    movq %rsp, %rbp
    movq 16(%rbp), %rsi
    movq %rsi, %rdi
.strlen_loop:
    cmpb $0, (%rsi)
    jz .strlen_done
    incq %rsi
    jmp .strlen_loop
.strlen_done:
    subq %rdi, %rsi
    movq %rsi, %rdx
    movq $1, %rax
    movq $1, %rdi
    movq 16(%rbp), %rsi
    syscall
    movq $1, %rax
    movq $1, %rdi
    leaq newline(%rip), %rsi
    movq $1, %rdx
    syscall
    movq %rdx, %rax
    incq %rax
    popq %rbp
    ret

	.data
shell:      .asciz "/bin/sh"
shell_c:    .asciz "-c"

	.text
	.global system
system:
    pushq %rbp
    movq %rsp, %rbp
	movq 16(%rbp), %rdi
    testq %rdi, %rdi
    jz .shell_available
    movq $57, %rax
    syscall
    testq %rax, %rax
    jz .child
    js .error
.parent:
    pushq %rdi
    movq %rax, %rdi     
    movq $0, %rsi       
    movq $0, %rdx        
    movq $61, %rax     
    syscall
    popq %rdi
    cmp $0, %rax
    jl .error
    shrl $8, %eax
    andl $0xFF, %eax
    jmp .exit
.child:
    subq $32, %rsp
    leaq shell(%rip), %rax
    movq %rax, 0(%rsp)   
    leaq shell_c(%rip), %rax
    movq %rax, 8(%rsp)  
    movq %rdi, 16(%rsp)   
    movq $0, 24(%rsp)    
    movq $59, %rax       
    leaq shell(%rip), %rdi
    movq %rsp, %rsi
    movq $0, %rdx
    syscall
    movq $60, %rax      
    movq $127, %rdi
    syscall
.shell_available:
    movq $1, %rax         
    jmp .exit
.error:
    movq $-1, %rax 
.exit:
    movq %rbp, %rsp
    popq %rbp
    ret

	.text
	.global sleep
sleep:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
	movq 16(%rbp), %rdi
    movq %rdi, 0(%rsp)   
    movq $0, 8(%rsp)     
    movq $35, %rax       
    movq %rsp, %rdi      
    movq $0, %rsi        
    syscall
    addq $16, %rsp
    movq %rbp, %rsp
    popq %rbp
    ret

	.text
	.globl usleep
usleep:
    pushq %rbp
    movq %rsp, %rbp
    movl 16(%rbp), %eax          
    movl $1000000, %ecx
    xorl %edx, %edx
    divl %ecx               
    imull $1000, %edx, %edx 
    subq $16, %rsp
    movq %rax, (%rsp)        
    movq %rdx, 8(%rsp)  
    movq $35, %rax           
    movq %rsp, %rdi          
    movq $0, %rsi            
    syscall
    addq $16, %rsp
    movq %rbp, %rsp
    popq %rbp
    ret

	.text
	.global kbhit
kbhit:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq $0, (%rsp)
    movq $1, %rax
    btsq $0, (%rsp)
    movq $0, 16(%rsp)
    movq $0, 24(%rsp)
    movq $23, %rax          
    movq $1, %rdi            
    movq %rsp, %rsi         
    movq $0, %rdx      
    movq $0, %r10           
    leaq 16(%rsp), %r8  
    syscall
    testq %rax, %rax
    setg %al
    movzbq %al, %rax
    addq $32, %rsp
    movq %rbp, %rsp
    popq %rbp
    ret

    .text
	.global getch
getch:
    pushq %rbp
    movq %rsp, %rbp
    subq $144, %rsp
    movq $0, (%rsp)
    movq $1, %rax
    btsq $0, (%rsp)
    movq $0, 16(%rsp)
    movq $0, 24(%rsp)
    movq $23, %rax
    movq $1, %rdi
    movq %rsp, %rsi
    movq $0, %rdx
    movq $0, %r10
    leaq 16(%rsp), %r8
    syscall
    testq %rax, %rax
    jle no_input
    movq $0, %rax
    movq $0, %rdi
    leaq -1(%rbp), %rsi
    movq $1, %rdx
    syscall
    testq %rax, %rax
    jle no_input
    movzbq -1(%rbp), %rax
    jmp done
no_input:
    movq $0, %rax
done:
    addq $144, %rsp
    movq %rbp, %rsp
    popq %rbp
    ret

.section .data
    rand_seed: .quad 1

.section .text
.global srand
.global rand
srand:
    pushq %rbp
    movq %rsp, %rbp
    movq 16(%rsp), %rdi
    movq %rdi, rand_seed
    popq %rbp
    ret

rand:
    pushq %rbp
    movq %rsp, %rbp
    movq rand_seed, %rax
    
    movq $1103515245, %rcx
    mulq %rcx
    addq $12345, %rax
    andq $0x7FFFFFFF, %rax
    movq %rax, rand_seed
    movq %rbp, %rsp
    popq %rbp
    ret

.section .text
.global time
time:
    pushq %rbp
    movq %rsp, %rbp
    movq $201, %rax
    movq 16(%rsp), %rdi
    syscall
    testq %rdi, %rdi
    jz time_done
    movq %rax, (%rdi)
time_done:
    movq %rbp, %rsp
    popq %rbp
    ret
