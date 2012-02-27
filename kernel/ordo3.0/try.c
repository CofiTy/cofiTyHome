#include "try.h"

/*
 * Save the current context and call the function f
 * parameters:
 * 	cont: where to save the context
 * 	fct: the function to call
 * return value:
 * 	- the f return value
 * 	- the second parameter of throw if f doesn't return (throw is called)
 */
int try(ctx *cont,fct *f)
{
	// save the ebp register
	asm("mov %%ebp, %0" :"=r"(cont->ebp));
	// save the esp register
	asm("mov %%esp, %0" :"=r"(cont->esp));
	// call the fuction f
	return f();
}

/*
 * Restore previous saved context
 * This function never returns
 * parameters:
 * 	cont: The context to restore
 * 	value: the value to be returned by the function try
 */
void throw(ctx *cont, int value)
{
	// Save the return value in temporary register
	asm("mov %0,%%edx"::"r"(value));
	// restore the esp register
	asm("mov %0,%%esp"::"r"(cont->esp));
	// restore the ebp register
	asm("mov %0,%%ebp"::"r"(cont->ebp));
	// set the return value
	asm("mov %%edx,%%eax"::"r"(value));
	// return
	asm("leave");
	asm("ret");
}
