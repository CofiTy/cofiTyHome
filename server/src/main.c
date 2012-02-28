#include <unistd.h>
#include <sys/epoll.h>
#include <unistd.h> //Pour STDIN_FILENO
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "common.h"
#include "logWatch.h"

#include "init.h"
#include "../parse/rules.tab.h"

#define MAX_EVENTS 10

int main(int argc, char ** argv) {


  init();

  struct epoll_event ev;
  memset(&ev, 0, sizeof (struct epoll_event));
  int epollfd;

  epollfd = epoll_create(10);
  FAIL(epollfd);

  //Ajout à epoll de l'entrée standard
  ev.events = EPOLLIN;
  ev.data.fd = STDIN_FILENO;
  FAIL(epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev));

  struct epoll_event events[MAX_EVENTS];

  int nfds;
  int done = 0;


  while (done == 0) //Boucle principale
  {

    do{
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    }while(errno == EINTR);
    
    FAIL(nfds);

    int n;

    for (n = 0; n < nfds; ++n) {
      if (events[n].data.fd == STDIN_FILENO) {
        char chaine[16];
        fgets(chaine, 15, stdin); //récupère l'entrée standard

        //Traite la commande
        if (strcmp(chaine, "exit\n") == 0 || strcmp(chaine, "quit\n") == 0 || strcmp(chaine, "q\n") == 0) {
          done = 1;
          puts("Quiting server!");
        } else if (strcmp(chaine, "reload\n") == 0 ||  strcmp(chaine, "update\n") == 0 || strcmp(chaine, "u\n") == 0){
          cleanMemory();
          parseAll();
        } else if (strcmp(chaine, "logR\n") == 0 ||  strcmp(chaine, "logRules\n") == 0 || strcmp(chaine, "lr\n") == 0){
          watchLogRules();
        } else if (strcmp(chaine, "logS\n") == 0 ||  strcmp(chaine, "logSensors\n") == 0 || strcmp(chaine, "ls\n") == 0){
          watchLogSensors();
        } else if (strcmp(chaine, "stopL\n") == 0 ||  strcmp(chaine, "stopLogs\n") == 0 || strcmp(chaine, "sl\n") == 0){
          stopWatchingLogs();
        } else if (strcmp(chaine, "clean\n") == 0 ||  strcmp(chaine, "clear\n") == 0 || strcmp(chaine, "cl\n") == 0){
          system("clear");
        } else if (strcmp(chaine, "launchT\n") == 0 ||  strcmp(chaine, "launchTime\n") == 0 || strcmp(chaine, "lt\n") == 0){
          startTimedCheck(60);
          puts("Timer Started");
        } else if (strcmp(chaine, "stopT\n") == 0 ||  strcmp(chaine, "stopTime\n") == 0 || strcmp(chaine, "st\n") == 0){
          stopTimedCheck();
          puts("Timer Stopped");
        }
        else {
          puts("Commande invalide\nUsage:");
          puts("  exit, quit, q");
          puts("  logR, logRules, lr");
          puts("  logS, logSensors, ls");
          puts("  stopL, stopLogs, sl");
          puts("  clean, clear, cl");
          puts("  launchT launchTime lt");
          puts("  stopT stopTime st");
        }
      }
    }
  }
  stopWatchingLogs();
  destroy();

  return 0;
}

