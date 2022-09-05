/* the code evolve from first_write_formal.c */

// replace switch_to
	__asm__(
		"cmpl %%ecx, current\n\t"
		"je 1f\n\t"
		/* save esp to current->tss.esp */
		"movl current, %%edi\n\t"
		"movl %%esp, 800(%%edi)\n\t"
		/* change PCB */
		"xchgl %%ecx, current\n\t"
		/* get new esp from current->tss.esp, note that now current is pointed to new task */
		"movl current, %%edi\n\t"
		"movl 800(%%edi), %%esp\n\t"

		/* load tr, ldt here */
		"shl $4, %%esi\n\t"
		"addw $40, %%si\n\t"
		"lldt %%si\n\t"	
		"ret\n\t" // first ret to first_ret_from_kernel
		"1: \n\t"
		::"c"((long)task[next]), "S"(next)
		);


// in copy_process 

	// p->tss.esp = (long)p + PAGE_SIZE - 800 above 
	// 800 is the value I choose randomly... 
	long *sp = p->tss.esp; 
	extern first_ret_from_kernel;
	*sp++ = &first_ret_from_kernel;
	*sp++ = eip;
	*sp++ = cs & 0xffff;
	*sp++ = eflags;
	*sp++ = esp;
	*sp++ = ss & 0xffff;

// in system_call.s
// remember declare global in header of the file
.align 2
first_ret_from_kernel:
	movl $0, %eax
	iret
