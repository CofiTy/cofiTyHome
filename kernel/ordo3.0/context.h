#ifndef CONTEXT_H
#define CONTEXT_H

typedef struct ctx
{
	struct ctx* next;
	struct ctx* last;
	int id;
	void* ebp;
	void* esp;
	void* stack;
	int time_to_wait;
	int zombi;
}ctx;

#endif
