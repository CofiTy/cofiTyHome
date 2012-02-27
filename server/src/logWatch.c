#include <stdio.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>  

#include "logWatch.h"
#include "common.h"


FILE *logRules;
FILE *logSensors;

int onGoingR = FALSE;
int onGoingS = FALSE;


FILE * watchLog(char * name){
  FILE *pp; 
  char command[32];
  sprintf(command, "tail -f %s", name);
  if((pp = popen(command, "w")) == NULL){ 
    perror("popen"); 
    exit(1);
  }
  return pp;
}

void stopWatching(FILE * pipe){
  pclose(pipe);
}

void watchLogRules(){
  if(onGoingR == FALSE){
    onGoingR = TRUE;
    logRules = watchLog(nameLogRules);
  }
}

void watchLogSensors(){
  if(onGoingS == FALSE){
    onGoingS = TRUE;
    logSensors = watchLog(nameLogSensors);
  }
}

void stopWatchingLogs(){
  system("killall -q tail");
  if(onGoingR == TRUE){
    stopWatching(logRules);
    onGoingR = FALSE;
  }
  if(onGoingS == TRUE){
    stopWatching(logSensors);
    onGoingS = FALSE;
  }
}
