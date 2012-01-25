#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include "guiNetwork.h"
#include "guiInterface.h"

typedef struct Client *PClient;

typedef struct Client {
  PClient next;
  int sock;
  pthread_t pthreadRec;
  pthread_t pthreadSend;
  mqd_t mqSend;
}Client;

typedef struct {
  PClient first;
  PClient current;
}List;

pthread_t pthreadConnexion;

List clientList;

void * guiMsgRec(void* data){

  char buff[128];
  char traite[128];
  int i, j, nb, total, blocs;
  int over = 0;
  char* receiving = (char *) buff[0];
  Client* client = (Client*)data;

  memset(buff, 0, 128);
  total = 0;
  blocs = 0;

  for(;;)
  {
    /* reception form sensors */
    nb = recv(client->sock, receiving, 128, 0);
    puts("recv");
    FAIL(nb);
    total += nb;
    receiving += nb;

    i = 0;
    j = 0;
    while(i < (strlen(buff) - 1)){
      traite[j++] = buff[i];
      if(buff[i] == '{'){
        ++blocs;
      } else if(buff[i] == '}'){
        --blocs;
      }
      if(blocs == 0){ /* If enough data we can process */
        puts("recv");
        /*Traiter traite*/
        processCommand(traite);
        j = 0;
        memset(traite, '\0', 128);
      }
      ++i;
    }

    total = 0;
    blocs = 0;
    over = 0;
    receiving = (char *) buff[0];
    memset(buff, '\0', 128);
  }
}

void * guiMsgSend(void* data){

  char buff[128];
  int nb, nbSent, total;
  char* sending = (char *) buff[0];
  Client* client = (Client*)data;

  for(;;)
  {
    /* Recuperation des messages de la boite au lettre "Envoi" */
    nb = mq_receive(client->mqSend, buff, 128, 0);
    puts("mq rec");
    FAIL(nb);

    total = nb;
    nbSent = 0;
    while(nbSent < total)
    {
      /* Sending toward sensors */
      nb = send(client->sock, sending, nb, 0);
      FAIL(nb);
      nbSent += nb;
      sending += nb;
    }
    puts("sent");
  }

}

void * guiNetworkConnexion(){

  int acceptSock, tmpSock;
  int i = 0;
  char name[32];
  struct mq_attr attr;

  struct sockaddr_in saddr_client;
  struct sockaddr_in saddr;
  socklen_t size_addr = sizeof(struct sockaddr_in);
  memset(&saddr_client, 0, sizeof(struct sockaddr_in));

  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(5003);

  acceptSock = socket(AF_INET, SOCK_STREAM, 0);
  FAIL(acceptSock);

  FAIL(bind(acceptSock, (struct sockaddr *)&saddr, sizeof(saddr)));

  FAIL(listen(acceptSock, 10));
  puts("Listening");

  for(;;){
    memset(&saddr, 0, sizeof(struct sockaddr_in));
    sprintf(name, "/mqMsgSendNb%d", i++);

    tmpSock = accept(acceptSock, (struct sockaddr *)&saddr_client, &size_addr);
    FAIL(tmpSock);
    puts("gotConnexion");

    if(clientList.first == NULL){
      clientList.first = malloc(sizeof(Client));
      clientList.first->next = NULL;
      clientList.current = clientList.first;
    } 
    else{
      clientList.current = malloc(sizeof(Client));
      clientList.current->next = clientList.first;
      clientList.first = clientList.current;
    }

    clientList.current->sock = tmpSock;

    puts(name);
    memset(&attr, 0, sizeof(struct mq_attr));
    attr.mq_flags = 0;
    attr.mq_maxmsg = 32;
    attr.mq_msgsize = 64;
    clientList.current->mqSend = mq_open(name, O_RDWR | O_CREAT, S_IRWXU, &attr);
    FAIL(clientList.current->mqSend);
    
    pthread_create(&clientList.current->pthreadRec, NULL, guiMsgRec, (void*)clientList.current);
    pthread_detach(clientList.current->pthreadRec);

    pthread_create(&clientList.current->pthreadSend, NULL, guiMsgSend, (void*)clientList.current);
    pthread_detach(clientList.current->pthreadSend);
  }
}

void guiNetworkStart(){
  clientList.first = NULL;
  pthread_create(&pthreadConnexion, NULL, guiNetworkConnexion, NULL);
  pthread_detach(pthreadConnexion);
}

void guiNetworkStop(){

  pthread_kill(pthreadConnexion, SIGTERM);
  while(clientList.first != NULL){
    pthread_kill(clientList.first->pthreadSend, SIGTERM);
    pthread_kill(clientList.first->pthreadRec, SIGTERM);
    close(clientList.first->sock);
    FAIL(mq_close(clientList.first->mqSend));

    clientList.current = clientList.first;
    clientList.first = clientList.first->next;
    free(clientList.current);
  }
}

int guiNetworkSend(const char * msg_ptr, size_t msg_len, mqd_t mqId){
  return mq_send( mqId, msg_ptr, msg_len, 0);
}
