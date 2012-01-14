#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <mqueue.h>
#include <signal.h>
#include <unistd.h>

#include "guiNetwork.h"

int guiSock;
pthread_t pthreadGUIRec, pthreadGUISend;
mqd_t mqGUISend;

void * guiMsgRec(){

  char buff[128];
  int nb, total;
  char* receiving = (char *) buff[0];

  memset(buff, 0, 128);
  total = 0;

  for(;;)
  {
    /* reception form sensors */
    nb = recv(guiSock, receiving, 128, 0);
    FAIL(nb)
      total += nb;
    receiving += nb;

    /* If enough data we can process */
    if(buff[total - 1] == '#')
    {
      puts("recv");
      total = 0;
      receiving = (char *) buff[0];
      memset(buff, 0, 128);
    }
  }
}

void * guiMsgSend(){

  char buff[128];
  int nb, nbSent, total;
  char* sending = (char *) buff[0];

  for(;;)
  {
    /* Recuperation des messages de la boite au lettre "Envoi" */
    nb= mq_receive(mqGUISend, buff, 128, 0);
    FAIL(nb)

      total = nb;
    nbSent = 0;
    while(nbSent < total)
    {
      /* Sending toward sensors */
      nb = send(guiSock, sending, nb, 0);
      FAIL(nb)
        nbSent += nb;
      sending += nb;
    }
    puts("sent");
  }

}

void guiNetworkStart(){

  int tmpSock;
  mqGUISend = mq_open("mqGUIMsgSend", O_RDWR | O_CREAT);

  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(struct sockaddr_in));

  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(5003);

  FAIL(bind(tmpSock, (struct sockaddr *)&saddr, sizeof(saddr)));

  FAIL(listen(tmpSock, 10));

  struct sockaddr_in saddr_client;
  memset(&saddr_client, 0, sizeof(struct sockaddr_in));

  socklen_t size_addr = sizeof(struct sockaddr_in);

  guiSock = accept(tmpSock, (struct sockaddr *)&saddr_client, &size_addr);
  FAIL(guiSock)

    pthread_create(&pthreadGUIRec, NULL, guiMsgRec, NULL);
  pthread_detach(pthreadGUIRec);

  pthread_create(&pthreadGUISend, NULL, guiMsgSend, NULL);
  pthread_detach(pthreadGUISend);

}

void guiNetworkStop(){

  pthread_kill(pthreadGUIRec, SIGTERM);
  pthread_kill(pthreadGUISend, SIGTERM);
  close(guiSock);
}

int guiNetworkSend(const char * msg_ptr, size_t msg_len){
  return mq_send( mqGUISend, msg_ptr, msg_len, 0);
}
