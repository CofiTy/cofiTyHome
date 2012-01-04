#ifndef kernel_myThreads_h
#define kernel_myThreads_h
#include <setjmp.h>



typedef struct Thread
{  
    int id;
    void* stack[200];
    int stack_size;
    jmp_buf ctx;
    struct Thread *next;
}Thread;

void init_threading_system();
void createThread(void(*function)(void));
void yield();
void launch_threads();

#endif
