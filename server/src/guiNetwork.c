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
#include "common.h"

#include "../../kernel/memory/memory.h"

typedef struct Client *PClient;

/**
 * Structure GUI Client Informations
 **/
typedef struct Client {
  PClient next;
  int sock;
  pthread_t pthreadRec;
  pthread_t pthreadSend;
  mqd_t mqSend;
}Client;

/**
 * Structure List of GUI Clients
 **/
typedef struct {
  PClient first;
  PClient current;
}List;

/**
 * Thread ID of connexion manager Thread
 **/
pthread_t pthreadConnexion;

/**
 * List of GUI CLients
 **/
List clientList;

/**
 * Thread Function: Receive GUI Message
 **/
void * guiMsgRec(void* data){

  char buff[8192];
  char traite[8192];
  int i, j, nb, total, blocs;
  int over = 0;
  char* receiving = (char *) buff;
  Client* client = (Client*)data;

  memset(buff, '\0', 8192);
  memset(traite, '\0', 8192);
  total = 0;
  blocs = 0;

  for(;;)
  {
    /* reception form sensors */
    nb = recv(client->sock, receiving, 8192, 0);
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
      /* If enough data we can process */
      if(blocs == 0){ 
        processCommand(traite, client->mqSend);
        j = 0;
        memset(traite, '\0', 8192);
      }
      ++i;
    }

    total = 0;
    blocs = 0;
    over = 0;
    receiving = (char *) buff;
    memset(buff, '\0', 8192);
  }
}

/**
 * Thread Function: Sending GUI Message
 **/
void * guiMsgSend(void* data){

  char buff[8192];
  int nb, nbSent, total;
  char* sending = (char *) buff;
  Client* client = (Client*)data;
  memset(buff, '\0', 8192);

  for(;;)
  {
    /* Read Messages in MQueue */
    nb = mq_receive(client->mqSend, buff, 8192, NULL);
    FAIL(nb);
    
    total = nb;
    nbSent = 0;
    while(nbSent < total)
    {
      /* Sending toward GUI Client */
      nb = send(client->sock, sending, nb, 0);
      FAIL(nb);
      nbSent += nb;
      sending += nb;
    }
    memset(buff, '\0', 8192);
    sending = (char *) buff;
  }

}

/**
 * Thread Function: Manage GUI Connexions
 **/
void * guiNetworkConnexion(){

  int acceptSock, tmpSock;
  int i = 0;
  char name[32];

  struct sockaddr_in saddr_client;
  struct sockaddr_in saddr;
  socklen_t size_addr = sizeof(struct sockaddr_in);
  memset(&saddr_client, 0, sizeof(struct sockaddr_in));

  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(lisPort);

  /* Create The listening socket */
  acceptSock = socket(AF_INET, SOCK_STREAM, 0);
  FAIL(acceptSock);

  /* Bind Socket */
  FAIL(bind(acceptSock, (struct sockaddr *)&saddr, sizeof(saddr)));


  for(;;){
    memset(&saddr, 0, sizeof(struct sockaddr_in));
    
    /* Creation of MQueue name */
    sprintf(name, "/mqMsgSendNb%d", i++);
  
    /* Listen */
    FAIL(listen(acceptSock, 10));
    puts("Listening");

    /* Accept GUI connexion */
    tmpSock = accept(acceptSock, (struct sockaddr *)&saddr_client, &size_addr);
    FAIL(tmpSock);
    puts("gotConnexion");

    /* Put Create Client Information in list */
    if(clientList.first == NULL){
      clientList.first = gMalloc(sizeof(Client));
      clientList.first->next = NULL;
      clientList.current = clientList.first;
    } 
    else{
      clientList.current = gMalloc(sizeof(Client));
      clientList.current->next = clientList.first;
      clientList.first = clientList.current;
    }

    clientList.current->sock = tmpSock;
    
    /* Create Client MQ */
    clientList.current->mqSend = mq_open(name, O_RDWR | O_CREAT, S_IRWXU, NULL);
    FAIL(clientList.current->mqSend);

    /* Launch Client Reception Thread */
    pthread_create(&clientList.current->pthreadRec, NULL, guiMsgRec, (void*)clientList.current);
    pthread_detach(clientList.current->pthreadRec);

    /* Launch Client Sending Thread */
    pthread_create(&clientList.current->pthreadSend, NULL, guiMsgSend, (void*)clientList.current);
    pthread_detach(clientList.current->pthreadSend);
  }
}

/**
 * Initialisation of the GUI Network
 **/
void guiNetworkStart(){
  clientList.first = NULL;
  pthread_create(&pthreadConnexion, NULL, guiNetworkConnexion, NULL);
  pthread_detach(pthreadConnexion);
}

/**
 * Desactivation of GUI Network and free related memory
 **/
void guiNetworkStop(){

  FAIL(pthread_cancel(pthreadConnexion));

  while(clientList.first != NULL){
    FAIL(pthread_cancel(clientList.first->pthreadSend));
    FAIL(pthread_cancel(clientList.first->pthreadRec));
    close(clientList.first->sock);
    FAIL(mq_close(clientList.first->mqSend));

    clientList.current = clientList.first;
    clientList.first = clientList.first->next;
    gFree(clientList.current);
  }
  puts("Closed");
}

/**
 * Send Message toward GUI Client
 **/
int guiNetworkSend(const char * msg_ptr, size_t msg_len, mqd_t mqId){
  return mq_send( mqId, msg_ptr, msg_len, 0);
}
