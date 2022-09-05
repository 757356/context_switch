
// remember to declare in system_call.s!
// compile with or without -finline-functions and other flags in kernel/Makefile is all OK 

# call with myswitch_to(next) in schedule()
/* in system_call.s */
.align 2
myswitch_to:
	pushl %ebp
	mov %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	
	movl 8(%ebp), %esi /* get arg of next(In the origin code, I use ESP to get it */
	movl task(, %esi, 4), %ecx /* get next task */
	cmpl %ecx, current
	je 1f
	
	/* save old task's(current) esp to current->tss.esp */
	movl current, %edi
	movl %esp, 800(%edi) /* offset is 800 from current->state */
	
	/* change PCB */
	xchgl %ecx, current
	
	/* get new task's(current) esp from current->tss.esp, 
	note that now current is pointed to new task */
	movl current, %edi
	movl 800(%edi), %esp

	/* set task[0]->tss.esp0 to current's kernel stack,
		interrupt needs it! */
	/* note that task[0] is the init_task, and TR point it in sched_init(),
		we don't change the TR, so we need to save new kernel stack pointer 
		to it */
	movl task, %ebx
	movl %edi, 748(%ebx) /* eps0 if offset 748 from task[0]->state */
	addl $4096, 748(%ebx) /* the value saved is £¨long)current + 4096 */ 

	/* load new ldt, note that ESI is next */
	shl $4, %esi
	addw $40, %si
	lldt %si	/* ldt description is in gdt[5 + next*2] */
	
	/* fs must update to point to new task's user space! */
	/* ds, cs no need to update */
	movl $0x17, %esi
	movw %si, %fs
1: 
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	 /* if the task is running first time, 
	 ret to first_ret_from_kernel, else
	 ret to schedule */
	ret

// in copy_process() 
	long *sp = (long)p + PAGE_SIZE;
	extern long first_ret_from_kernel;
	*--sp = ss & 0xffff;
	*--sp = esp;
	*--sp = eflags;
	*--sp = cs & 0xffff;
	*--sp = eip;
	/* the order is equal to the order in copy_process's arg */
	*--sp = ds & 0xffff;
	*--sp = es & 0xffff;
	*--sp = fs & 0xffff;
	*--sp = edx;
	*--sp = ecx;
	*--sp = ebx;
	*--sp = gs & 0xffff;
	*--sp = esi;
	*--sp = edi;
	*--sp = ebp;
	*--sp = 0;
	
	*--sp = &first_ret_from_kernel;
	 /* this is the args poped by myswitch_to when it return to every new task first time*/
	 /* in fact, if I adjust the order of registers in the stack, this is not need */
	 /* note that EAX, EBP, EDI, ESI will be update immediately
	 in first_ret_from_kernel. the value here is useless */
	sp -= 4;
	
	p->tss.esp = sp;

/* in system_call.s */	
.align 2
first_ret_from_kernel:
	popl %eax
	popl %ebp
	popl %edi
	popl %esi
	popl %gs
	popl %ebx
	popl %ecx
	popl %edx
	popl %fs
	popl %es
	popl %ds
	iret
