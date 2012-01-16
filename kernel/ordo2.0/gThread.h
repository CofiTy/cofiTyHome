#ifndef GTHREAD_H_
#define GTHREAD_H_

#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

/* machine context data structure */
/* save machine context */
#define mctx_save(mctx) setjmp((mctx)->jb)

/* restore machine context */
#define mctx_restore(mctx) longjmp((mctx)->jb, 1)

/* switch machine context */
#define mctx_switch(mctx_old,mctx_new)  \
	if (setjmp((mctx_old)->jb) == 0)  \
		longjmp((mctx_new)->jb, 1)

typedef struct mctx_st
{
	jmp_buf jb;
}mctx_t;


void mctx_create(mctx_t *mctx, void (*sf_addr)(void*),
		void *sf_arg, void *sk_addr, size_t sk_size);




#endif /* GTHREAD_H_ */
