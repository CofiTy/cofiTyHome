#include <unistd.h>
#include <sys/epoll.h>
#include <unistd.h> //Pour STDIN_FILENO

#include "init.h"

#define MAX_EVENTS 10

int main(int argc, char ** argv){
	
	init();
	
  struct epoll_event ev;
  memset(&ev, 0, sizeof(struct epoll_event));
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

  
  while(done == 0) //Boucle principale
  {

    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    FAIL(nfds);

    int n;

    for (n = 0; n < nfds; ++n)
    {
      if(events[n].data.fd == STDIN_FILENO)
      {
        char chaine[16];
        fgets(chaine, 15, stdin); //récupère l'entrée standard

        //Traite la commande
        if(strcmp(chaine, "exit\n") == 0 || strcmp(chaine, "quit\n") == 0 || strcmp(chaine, "q\n") == 0)
        {
          done = 1;
          puts("Quiting server!");
        }
        else
        {
          puts("Commande invalide\nUsage: exit, quit, q");
        }
        done = 1;
      }
    }
  }
  destroy();

  return 0;
}

