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

static mctx_t  		mctx_caller;
static sig_atomic_t	mctx_called;

static mctx_t		*mctx_creat;
static void		   (*mctx_creat_func)(void *);
static void			*mctx_creat_arg;
static sigset_t		 mctx_creat_sigs;

void mctx_creat_trampoline(int sig);
void mctx_creat_boot(void);

mctx_t mainContext;
mctx_t child1Context;

void mctx_create(mctx_t *mctx, void (*sf_addr)(void*),
		void *sf_arg, void *sk_addr, size_t sk_size)
{
	struct sigaction sa;
	struct sigaction osa;
	struct sigaltstack ss;
	struct sigaltstack oss;
	sigset_t osigs;
	sigset_t sigs;

	/* step 1 */
	sigemptyset(&sigs);
	sigaddset(&sigs,SIGUSR1);
	sigprocmask(SIG_BLOCK, &sigs, &osigs);

	/* step 2 */
	memset((void*)&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = mctx_creat_trampoline;
	sa.sa_flags = SA_ONSTACK;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGUSR1, &sa, &osa);

	/* step 3 */
	ss.ss_sp   = sk_addr;
	ss.ss_size = sk_size;
	ss.ss_flags = 0;
	sigaltstack(&ss, &oss);

	/* step 4 */
	mctx_creat = mctx;
	mctx_creat_func = sf_addr;
	mctx_creat_arg = sf_arg;
	mctx_creat_sigs = osigs;
	mctx_called = FALSE;
	kill(getpid(), SIGUSR1);
	sigfillset(&sigs);
	sigdelset(&sigs, SIGUSR1);
	while (!mctx_called)
	{
		sigsuspend(&sigs);
	}

	/* step 6 */
	sigaltstack(NULL, &ss);
	ss.ss_flags = SS_DISABLE;
	sigaltstack(&ss, NULL);
	if (!(oss.ss_flags & SS_DISABLE))
	{
		sigaltstack(&oss, NULL);
	}
	sigaction(SIGUSR1,&osa, NULL);
	sigprocmask(SIG_SETMASK, &osigs, NULL);

	/* step 7 & 8 */
	mctx_switch(&mctx_caller, mctx);

	/* step 14 */
	return;
}

void mctx_creat_trampoline(int sig)
{
	/* step 5 */
	if (mctx_save(mctx_creat) == 0)
	{
		mctx_called = TRUE;
		return;
	}

	/* step 9 */
	mctx_creat_boot();
}

void mctx_creat_boot(void)
{
	void (*mctx_start_func)(void *);
	void *mctx_start_arg;

	/* step 10 */
	sigprocmask(SIG_SETMASK, &mctx_creat_sigs, NULL);

	/* step 11 */
	mctx_start_func = mctx_creat_func;
	mctx_start_arg = mctx_creat_arg;

	/* step 12 & 13 */
	mctx_switch(mctx_creat, &mctx_caller);

	/* The thread starts */
	mctx_start_func(mctx_start_arg);

	/* NOTREACHED */
	abort();

}

void child()
{
	for (;;)
	{
		printf("child\n");
		sleep(1);
		mctx_save(&child1Context);
		mctx_switch(&child1Context,&mainContext);
	}
}

int main()
{
	char stack[1024];
	mctx_create(&child1Context, &child,NULL, stack, 1024);
	for (;;)
	{
		printf("main\n");
		sleep(1);
		mctx_save(&mainContext);
		mctx_switch(&mainContext,&child1Context);
	}
	return 1;
}
