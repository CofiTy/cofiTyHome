#include "scheduler.h"
#include "hw.h"

// idle function
static void idle();

// message queue to manage zombi
static int wait_mq;

// linked list of sleeping thread
extern ctx *thread_in_waiting;

// active thread
extern ctx *current_thread;

// thread table
extern ctx *threads[256];

// create and initialise a new context
static void init_ctx(ctx *cont, int stack_size, fct2* func);

// suicide
static void delete_current_thread();

// handler interrupt
static void yield();

// switch between two context
static void switch_to_ctx(ctx* to_save, ctx* to_load);

// send sigkill to a thread
void kill(int pid)
{
	// enter in safe mode
	disable_it();
	// check if the pid is ok
	if ((unsigned)pid > 255 || threads[pid] == (ctx*)0)
	{
		printf("Le thread numero %d n'existe pas",pid);
	}
	else
	{
		// set the flag
		threads[pid]->zombi = 1;
	}
	// leave safe mode
	enable_it();
}

/*
 * suspend the current thread
 * parameter:
 * 	seconds: time to sleep in second
 */
void sleep(int seconds)
{
	ctx *toSleep, *iter, *prev;
	// enter in safe mode
	disable_it();
	// set the waik-up time
	current_thread->time_to_wait = time(NULL) + seconds;
	// chain the thread at the good place
	prev = NULL;
	iter = thread_in_waiting;
	while(iter != NULL && iter->time_to_wait < current_thread->time_to_wait)
	{
		prev = iter;
		iter = iter->next;
	}
	toSleep = current_thread;
	disable_current_thread();
	toSleep->next = iter;

	if(prev == NULL)
	{
		thread_in_waiting = toSleep;
	}
	else
	{
		prev->next = toSleep;
	}
	// call the scheduler
	scheduler(toSleep);
}

// suicide
void delete_current_thread()
{
	ctx garbage;
	// enter in safe mode
	disable_it();
	// send message to signal suicide
	send_message(wait_mq,(char*)&(current_thread->id),4);
	// remove the thread from activable ones
	disable_current_thread();
	// call the scheduler
	scheduler(&garbage);
}


/*
 * Remove the current thread from the activable threads
 */
void disable_current_thread()
{
	ctx *save;
	// enter in safe mode
	disable_it();
	save = current_thread;
	current_thread = save->next;
	current_thread->last = save->last;
	current_thread->last->next = current_thread;
	save->next = NULL;
	save->last = NULL;
	// leave safe mode
	enable_it();
}

/*
 * Start the threading system
 */
void start_scheduler()
{
	int i;
	ctx* new;
	thread_in_waiting = NULL;
	// initialize the thread table
	for (i=0;i<256;threads[i++]=0);
	// create main thread
	new = malloc(sizeof(ctx));
	new->next = new;
	new->last = new;
	current_thread = new;
	asm("mov %%ebp, %0" :"=r"(new->ebp));
	asm("mov %%esp, %0" :"=r"(new->esp));
	new->zombi = 0;
	new->id = -1;
	// initialize protection system
	setup_irq(1,&yield);
	// create idle thread
	create_thread(1000000,&idle);
	// create message queue for signaling zombies
	wait_mq = new_message_queue();
	// start timer
	start_hw();
}

/*
 * Spawn a new thread
 * parameters:
 * 	stack_size: stack size to allocate
 * 	func: the thread function
 * return value: the new thread id
 */
int create_thread(int stack_size, fct2 *func)
{
	int id;
	ctx *new;
	// enter in safe mode
	disable_it();
	// allocate new thread structur
	new = malloc(sizeof(ctx));
	// chain the new thread in activable ones
	new->next = current_thread->next;
	new->last = current_thread;
	current_thread->next->last = new;
	current_thread->next = new;

	new->zombi = 0;
	// look for an available id
	for (id = 0; threads[id] != 0; id++);
	new->id = id;
	threads[new->id] = new;
	// initialise the context
	init_ctx(new,stack_size,func);
	// leave safe mode
	reset_it();
	return new->id;;
}

// set up the stack for start a new thread
// parameters:
// 	cont: context pointer
// 	stack_size: size to allocate for stack
// 	func: function to initalize in the context
void init_ctx(ctx *cont, int stack_size, fct2 *func)
{
	cont->stack = (void*)malloc(stack_size);
	// set the stack pointers
	cont->ebp = cont->stack + stack_size - 16;
	cont->esp = cont->stack + stack_size - 16;
	// set the function on the stack
	*(int*)(cont->stack + stack_size - 16) = (int)(cont->stack + stack_size - 8);
	*(int*)(cont->stack + stack_size - 12) = (int)func;
	// set the suicide function on the stack
	*(int*)(cont->stack + stack_size - 8) = (int)&delete_current_thread;
	*(int*)(cont->stack + stack_size - 4) = (int)(cont->stack + stack_size - 4);
}

// handler
void yield()
{
	ctx *thread_to_wake_up;
	int c_time = time(NULL);
	// enter in safe mode
	disable_it();

	// wake up all thread that need it
	while(thread_in_waiting != NULL && thread_in_waiting->time_to_wait <= c_time)
	{
		thread_to_wake_up = thread_in_waiting;
		thread_in_waiting = thread_in_waiting->next;
		// insert the thread to wake up in activable list
		thread_to_wake_up->last = current_thread;
		thread_to_wake_up->next = current_thread->next;
		current_thread->next->last = thread_to_wake_up;
		current_thread->next = thread_to_wake_up;
	}
	// switch to the next thread
	current_thread = current_thread->next;
	// call the scheduler
	scheduler(current_thread->last);

}

/*
 * Schedule threads
 * parameter:
 * 	to_save: the context to save before switching
 */
void scheduler(ctx* to_save)
{
	ctx *save;
	// enter in safe mode
	disable_it();

	// if the zombi flag is set the the thread kill itself
	// and switch to the next thread
	if (current_thread->zombi)
	{
		save = current_thread;
		send_message(wait_mq,(char*)&(current_thread->id),4);
		disable_current_thread();
		switch_to_ctx(to_save,current_thread);
	}
	// else simple switch to the next thread
	else
	{
		switch_to_ctx(to_save,current_thread);
	}
	// leave safe mode without conditions
	reset_it();
}

/*
 * wait for a deleted thread
 * return value: the id of the deleted thread
 */
int wait()
{
	int i;
	// block until a thread is killed
	receive_message(wait_mq,(char*)&i,4);
	// free this thread
	free(threads[i]->stack);
	free(threads[i]);
	threads[i] = (ctx*)0;
	// return the id of the killed thread
	return i;
}

// switch beween two contexts
void switch_to_ctx(ctx* to_save, ctx* to_load)
{
	// save stack pointers
	asm("mov %%ebp, %0" :"=r"(to_save->ebp));
	asm("mov %%esp, %0" :"=r"(to_save->esp));
	// restor stack poniters
	asm("mov %0, %%esp" ::"r"(to_load->esp));
	asm("mov %0, %%ebp" ::"r"(to_load->ebp));
	return;
}

// idle function
void idle()
{
	for(;;)
	{
		// call the scheduler
		yield();
	}
}
