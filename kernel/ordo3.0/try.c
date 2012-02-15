#include "try.h"

int try(ctx *cont,fct *f)
{
	asm("mov %%ebp, %0" :"=r"(cont->ebp));
	asm("mov %%esp, %0" :"=r"(cont->esp));
	return f();
}

void throw(ctx *cont, int value)
{
	asm("mov %0,%%edx"::"r"(value));
	asm("mov %0,%%esp"::"r"(cont->esp));
	asm("mov %0,%%ebp"::"r"(cont->ebp));
	asm("mov %%edx,%%eax"::"r"(value));
	asm("leave");
	asm("ret");
}
