#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <mqueue.h>
#include <signal.h>
#include <unistd.h>

#include "sensorsNetwork.h"
#include "sensors.h"
#include "common.h"

int i, j, sock;
pthread_t pthreadSensorsRec, pthreadSensorsSend;
mqd_t mqSensorsSend;

void * sensorsMsgRec(){

  char buff[128];
  char data[32];
  int nb, total;
  char* receiving = (char *) buff;

  memset(buff, '\0', 128);
  total = 0;

  for(;;)
  {
    /* reception form sensors */
    nb = recv(sock, buff, 128, 0);
    FAIL(nb);

    total += nb;
    receiving += nb;

    /* If enough data we can process */
    i = 0;
    j = 0;
    while(i < (strlen(buff))){
      data[j++] = buff[i++];
      if(j == 28){
        /*Traiter data*/
        printf("Trame : %s\n", data);
        decodeTrame(data);
        //puts("finished decoding");
        j = 0;
        memset(data, '\0', 32);
      }
    }

      total = 0;
      receiving = (char *) buff;
      memset(buff, '\0', 128);
  }
}

void * sensorsMsgSend(){

  char buff[8192];
  int nb, nbSent, total;
  char* sending = (char *) buff;

  for(;;)
  {
    /* Recuperation des messages de la boite au lettre "Envoi" */
    nb = mq_receive(mqSensorsSend, buff, 8192, 0);
    FAIL(nb);
    
    //printf("Sending toward Gateway: %s\n", buff);

    total = nb;
    nbSent = 0;
    while(nbSent < total)
    {
      /* Sending toward sensors */
      nb = send(sock, sending, nb, 0);
      FAIL(nb);
      nbSent += nb;
      sending += nb;
    }
    //puts("sent");
    sending = (char *) buff;
  }

}

void sensorsNetworkStart(){

  mqSensorsSend = mq_open("/SensorsMsgSend", O_RDWR | O_CREAT, S_IRWXU, NULL);
  FAIL(mqSensorsSend);

  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(struct sockaddr_in));

  saddr.sin_addr.s_addr = inet_addr(conIP); 

  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(conPort);

  socklen_t len = sizeof(struct sockaddr_in);

  sock = socket(AF_INET, SOCK_STREAM, 0);
  FAIL(sock);

  FAIL(connect(sock, (struct sockaddr *)&saddr, len));
  
  puts("Connected to gateway");
  pthread_create(&pthreadSensorsRec, NULL, sensorsMsgRec, NULL);
  pthread_detach(pthreadSensorsRec);

  pthread_create(&pthreadSensorsSend, NULL, sensorsMsgSend, NULL);
  pthread_detach(pthreadSensorsSend);

}

void sensorsNetworkStop(){

  FAIL(pthread_cancel(pthreadSensorsRec));
  FAIL(pthread_cancel(pthreadSensorsSend));
  FAIL(close(sock));
  FAIL(mq_close(mqSensorsSend));
  puts("Closed");
}

int sensorsNetworkSend(const char * msg_ptr, size_t msg_len){
  return mq_send( mqSensorsSend, msg_ptr, msg_len, 0);
}
