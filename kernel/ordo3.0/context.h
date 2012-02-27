#ifndef CONTEXT_H
#define CONTEXT_H

/*
 * This is the thread structure
 */
typedef struct ctx
{
	// Two pointers for chaining threads
	struct ctx* next;
	struct ctx* last;
	
	// Thread Id
	int id;

	// Saved stack base pointer
	void* ebp;

	// Saved stack pointer
	void* esp;

	// A pointer to the allocated stack
	void* stack;

	// Usefull for the function Sleep
	int time_to_wait;

	// Boolean to notify to the thread that its suicide is needed
	int zombi;
}ctx;

#endif
