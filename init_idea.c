/* the most origin idea of context switch */
int main()
{
	int current;
	__asm__ volatile(
			"movl $current, %%ecx\n\t"
			"addl 744(%%ecx), %%ecx\n\t" //current->tss
			"movw $0, (%%ecx)\n\t" // last tss

		"movl %%esp, 4(%%ecx)\n\t" // esp0 maybe bug...
		"movw $0x10, 8(%%ecx)\n\t" // ss0

		"movl $0, 12(%%ecx)\n\t" // esp1
		"movw $0, 16(%%ecx)\n\t" // ss1
		"movl $0, 20(%%ecx)\n\t" // esp2
		"movw $0, 24(%%ecx)\n\t" // ss2

		//"movl %%\n\t"  // cr3
		//"\n\t" // eip

		"pushl %%edx\n\t"
		"pushf \n\t"
		"popl %%edx\n\t"
		"movl %%edx, 36(%%ecx)\n\t" // eflags
		"popl %%edx\n\t"

			"movl %%eax, 40(%%ecx)\n\t" 
			"movl $current, 44(%%ecx)\n\t"// ecx
			"movl %%edx, 48(%%ecx)\n\t" 
			"movl %%ebx, 52(%%ecx)\n\t" 
			"movl %%esp, 56(%%ecx)\n\t" // esp
			"movl %%ebp, 60(%%ecx)\n\t" // ebp
			"movl %%esi, 64(%%ecx)\n\t"
			"movl %%edi, 68(%%ecx)\n\t"
			"movw %%es,  72(%%ecx)\n\t"
			"movw %%cs,  76(%%ecx)\n\t"
			"movw %%ss,  80(%%ecx)\n\t"
			"movw %%ds,  84(%%ecx)\n\t"
			"movw %%fs,  88(%%ecx)\n\t"
			"movw %%gs,  92(%%ecx)\n\t"
		//	"\n\t" // ldt selector
			"movl $0x80000000, 100(%%ecx)\n\t" // i/o map and t...
			::"d"(_LDT(xxx)), "c"((long)task[next])
			);
			
			
			// change to next.....
			__asm__ volatile(
			// ecx should point to task[next] 
			"xchg %%ecx, current\n\t"	
			
			"\n\t" //tss
			"\n\t"  //esp0
			"\n\t" // ss0
			"\n\t" // esp1
			"\n\t" // ss1
			"\n\t" //esp2
			"\n\t" //ss2
			"\n\t" //cr3
			"\n\t"  //eip
			
			// ecx should point to task[next] now
			"movl 36(%%ecx), %%eax\n\t" // eflags
			"pushl %%eax\n\t"
			"popf \n\t"
			
			"movl 40(%%ecx), %%eax\n\t" // eax
			"movl 44(%%ecx), " // ecx
			"movl 48(%%ecx), %%edx\n\t" //edx
			"movl 52(%%ebx), %%ebx\n\t" // ebx 
			""  // esp
			"movl 60(%%ecx), %%ebp\n\t" // ebp
			"movl 64(%%ecx), %%esi\n\t" // esi
			"movl 68(%%ecx), %%edi\n\t" // edi
			
			// ldt
			"\n\t"
			
			"movl 72(%%ecx), %%es\n\t" 
			
			"movl \n\t"  // cs
			
			"movl \n\t"  // ss
			"movl 84(%%ecx), %%ds\n\t" 
			"movl 88(%%ecx), %%fs\n\t" 
			"movl 92(%%ecx), %%gs\n\t" 
					::);
	
}
