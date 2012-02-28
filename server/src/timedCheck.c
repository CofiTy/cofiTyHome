#include <unistd.h>
#include <signal.h>

#include "timedCheck.h"

int sleepTime;

void catchedAlarm(int sig_num) {
      sem_post(&checkRules);
      alarm(sleepTime);
}

void catchedAlarmDone(int sig_num) {
}


void startTimedCheck(int nbSec){
  signal(SIGALRM, catchedAlarm);
  sleepTime = nbSec;
  alarm(sleepTime);
}

void stopTimedCheck(){
  signal(SIGALRM, catchedAlarmDone);
}
