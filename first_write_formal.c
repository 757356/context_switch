/* the code I first to try to context switch formal */
	__asm__(
		"cmpl %%ecx, current\n\t"
		"je 1f\n\t"
		/* save esp to current->tss.esp */
		"movl current, %%edi\n\t"
		"movl %%esp, 800(%%edi)\n\t"
		
		"xchgl %%ecx, current\n\t"
		/* get esp to eax */
		"movl current, %%edi\n\t"
		"movl 800(%%edi), %%ecx\n\t"
		/* prepare for iret */
		"pushl $0x10\n\t"
		"pushl %%ecx\n\t"
		"pushfl \n\t"
		"pushl $0x8\n\t"
		"pushl $2f\n\t"
		"iret \n\t"

		"2:\n\t" // load tr, ldt here
		"1: \n\t"
		::"c"((long)task[next]));
