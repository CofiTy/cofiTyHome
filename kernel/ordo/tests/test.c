#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

void my_handler();

int main(void)
{

  timer_t timerid;
  struct itimerspec value;

  struct sigevent sev;
  struct sigaction sa;

  value.it_value.tv_sec = 5;
  value.it_value.tv_nsec = 0;

  value.it_interval.tv_sec = 5;
  value.it_interval.tv_nsec = 0;


  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = my_handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGRTMIN, &sa, NULL);
  
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGRTMIN;
  sev.sigev_value.sival_ptr = &timerid;
  
  timer_create (CLOCK_REALTIME, &sev, &timerid);

  printf("timer ID is 0x%lx\n", (long) timerid);

  //timer_connect (timerid, my_handler,0);

  timer_settime (timerid, 0, &value, NULL);

  for(;;)
  {
      printf("main\n");
      sleep(1);
  }

  return 0;

}

void my_handler()
{
  printf("\nHello World\n");
}
