// kernel/Makefile compile without -finline-functions and other flags
// remember to declare in system_call.s!


// replace switch_to in schedule()
	__asm__(
		"cmpl %%ecx, current\n\t"
		"je 1f\n\t"
		/* save old esp */
		"movl current, %%edi\n\t"
		"movl %%esp, 800(%%edi)\n\t"
		/* change PCB */
		"xchgl %%ecx, current\n\t"
		/* get new esp */
		"movl current, %%edi\n\t"
		"movl 800(%%edi), %%esp\n\t"

		/* kernel stack pointer use by interrupted */
		"movl task, %%ebx\n\t"
		"movl %%edi, 748(%%ebx)\n\t"
		"addl $4096, 748(%%ebx)\n\t"

		/* load ldt */
		"shl $4, %%esi\n\t"
		"addw $40, %%si\n\t"
		"lldt %%si\n\t"	
		
		/* fs point to user space */
		"movl $0x17, %%esi\n\t"
		"movw %%si, %%fs\n\t"
		
		/* see asm of schedule() to know why here is like this */
		"movl %%esp, %%ebp\n\t"
		"addl $12, %%ebp\n\t"
		"1: \n\t"
		::"c"((long)task[next]), "S"(next), "D"(current), "b"(task)
		);

// in copy_process()
	p->tss.esp = (long)p + PAGE_SIZE;
	long *sp = p->tss.esp;
	extern long first_ret_from_kernel;
	*--sp = ss & 0xffff;
	*--sp = esp;
	*--sp = eflags;
	*--sp = cs & 0xffff;
	*--sp = eip;
	
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
	
	/* value poped by schedule() */
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
