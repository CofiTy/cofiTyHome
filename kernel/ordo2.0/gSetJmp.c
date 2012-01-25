#include "gSetJmp.h"

int  gSetJmp(gBuffer buffer)
{
	volatile char* ebp;
	asm("mov %%ebp, %0" : "=r" (ebp));
	buffer[0] = ((int*)ebp)[1]; /* return adress*/
	buffer[1] = (int)ebp;
	return 0;
}

void gLongJmp(gBuffer buffer,int value)
{
	asm("mov %0, %%esp" :: "r" (buffer[1]+8));
	asm("mov %0, %%eax" :: "r" (value));
	asm("mov %0, %%edx" :: "r" (buffer[0]));
	asm("mov %0, %%ebp" :: "r" (*((int*)buffer[1])));
	asm("jmp *%edx");
}
