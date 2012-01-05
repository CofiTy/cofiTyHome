#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <setjmp.h>

static jmp_buf buf;
 
void signal_handler (int x)
{
    printf ("Signal with the following number: %d\n", x);
    /* Go back to where setjmp was called. Return the value "1" from
       setjmp. */
    longjmp (buf,1);
}

int main (int argc, char ** argv)
{
    

    /* Set an alarm to go off after 1,000 microseconds (one thousandth
       of a second). */
    struct sigaction act;
    act.sa_handler = signal_handler;
    sigaction (SIGALRM, & act, 0);

    for(;;) {
    
    ualarm (1000, 1000);
    if (! setjmp (buf)) {
      printf("Délire\n");
      sleep(1);
    } else {
      /* This branch is reached from the function "signal_handler"
	 above. */
      printf ("Jumped to here.\n");
    }
    }
    return 0;
}
