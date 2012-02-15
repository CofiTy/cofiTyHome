#include "scheduler.h"
#include "hw.h"

static void idle();
static int wait_mq;
extern ctx *thread_in_waiting;
extern ctx *current_thread;
extern ctx *threads[256];
static void init_ctx(ctx *cont, int stack_size, fct2* func);
static void delete_current_thread();
static void yield();
static void switch_to_ctx(ctx* to_save, ctx* to_load);
void kill(int pid)
{
	disable_it();
	if ((unsigned)pid > 255 || threads[pid] == (ctx*)0)
	{
		printf("Le thread numero %d n'existe pas",pid);
	}
	else
	{
		threads[pid]->zombi = 1;
	}
	enable_it();
}
void sleep(int seconds)
{
	ctx *toSleep, *iter, *prev;
	disable_it();
	current_thread->time_to_wait = time(NULL) + seconds;
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
	scheduler(toSleep);
}
void delete_current_thread()
{
	ctx garbage;
	printf("in delete\n");
	disable_it();
	send_message(wait_mq,(char*)&(current_thread->id),4);
	disable_current_thread();
	scheduler(&garbage);
}



void disable_current_thread()
{
	ctx *save;
	disable_it();
	save = current_thread;
	current_thread = save->next;
	current_thread->last = save->last;
	current_thread->last->next = current_thread;
	save->next = NULL;
	save->last = NULL;
	enable_it();
}

void start_scheduler()
{
	int i;
	ctx* new;
	thread_in_waiting = NULL;
	for (i=0;i<256;threads[i++]=0);
	new = malloc(sizeof(ctx));
	new->next = new;
	new->last = new;
	current_thread = new;
	asm("mov %%ebp, %0" :"=r"(new->ebp));
	asm("mov %%esp, %0" :"=r"(new->esp));
	new->zombi = 0;
	new->id = -1;
	setup_irq(1,&yield);
	create_thread(1000000,&idle);
	wait_mq = new_message_queue();
	start_hw();
}


int create_thread(int stack_size, fct2 *func)
{
	int id;
	ctx *new;
	disable_it();
	new = malloc(sizeof(ctx));
	new->next = current_thread->next;
	new->last = current_thread;
	current_thread->next->last = new;
	current_thread->next = new;

	new->zombi = 0;
	for (id = 0; threads[id] != 0; id++);
	new->id = id;
	threads[new->id] = new;
	init_ctx(new,stack_size,func);
	enable_it();
	return new->id;;
}


void init_ctx(ctx *cont, int stack_size, fct2 *func)
{
	cont->stack = (void*)malloc(stack_size);
	cont->ebp = cont->stack + stack_size - 16;
	cont->esp = cont->stack + stack_size - 16;
	*(int*)(cont->stack + stack_size - 16) = (int)(cont->stack + stack_size - 8);
	*(int*)(cont->stack + stack_size - 12) = (int)func;
	//*(int*)(cont->stack + stack_size - 8) = (int)(cont->stack + stack_size - 4);
	*(int*)(cont->stack + stack_size - 8) = (int)&delete_current_thread;
	*(int*)(cont->stack + stack_size - 4) = (int)(cont->stack + stack_size - 4);
}


void yield()
{
	ctx *thread_to_wake_up;
	int c_time = time(NULL);
	disable_it();
	//printf("%d YIELD\n",current_thread->id);

	while(thread_in_waiting != NULL && thread_in_waiting->time_to_wait <= c_time)
	{
		thread_to_wake_up = thread_in_waiting;
		thread_in_waiting = thread_in_waiting->next;

		thread_to_wake_up->last = current_thread;
		thread_to_wake_up->next = current_thread->next;
		current_thread->next->last = thread_to_wake_up;
		current_thread->next = thread_to_wake_up;
	}
	current_thread = current_thread->next;
	//printf("On appelle schduler pour le thread %d\n",current_thread->id);
	//printf("On appelle schduler pour le thread %d\n",current_thread->zombi);
	scheduler(current_thread->last);

}


void scheduler(ctx* to_save)
{
	ctx *save;
	disable_it();

	if (current_thread->zombi)
	{
		save = current_thread;
		send_message(wait_mq,(char*)&(current_thread->id),4);
		disable_current_thread();
		switch_to_ctx(to_save,current_thread);
	}
	else
	{
		switch_to_ctx(to_save,current_thread);
	}
	reset_it();
}

int wait()
{
	int i;
	receive_message(wait_mq,(char*)&i,4);
	free(threads[i]->stack);
	free(threads[i]);
	threads[i] = (ctx*)0;

	return i;
}

void switch_to_ctx(ctx* to_save, ctx* to_load)
{
	asm("mov %%ebp, %0" :"=r"(to_save->ebp));
	asm("mov %%esp, %0" :"=r"(to_save->esp));
	asm("mov %0, %%esp" ::"r"(to_load->esp));
	asm("mov %0, %%ebp" ::"r"(to_load->ebp));
	return;
}

void idle()
{
	for(;;)
	{
		yield();
	}
}
