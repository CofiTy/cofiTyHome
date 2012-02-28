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

int i, j;

/**
 * Full Duplex Socket for Sensors communication
 **/
int sock;

/**
 * Threads ID, reception and invoice for sensors communication
 **/
pthread_t pthreadSensorsRec, pthreadSensorsSend;

/**
 * Message queue ID for invoice messages toward Sensors
 **/
mqd_t mqSensorsSend;

/**
 * Thread Function: Receive Sensors Message
 **/
void * sensorsMsgRec(){

  char buff[128];
  char data[32];
  int nb;

  memset(buff, '\0', 128);
  j = 0;

  for(;;)
  {
    /* reception form sensors */
    nb = recv(sock, buff, 128, 0);
    FAIL(nb);

    i = 0;
    while(i < (strlen(buff))){
      data[j++] = buff[i++];
      /* If enough data we can process */
      if(j == 28){
        decodeTrame(data);
        j = 0;
        memset(data, '\0', 32);
      }
    }

    memset(buff, '\0', 128);
  }
}

/**
 * Thread Function: Sending Messages Toward Sensors
 **/
void * sensorsMsgSend(){

  char buff[8192];
  int nb, nbSent, total;
  char* sending = (char *) buff;

  for(;;)
  {
    /* Read Messages in MQueue */
    nb = mq_receive(mqSensorsSend, buff, 8192, 0);
    FAIL(nb);

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
    sending = (char *) buff;
  }

}

/**
 * Initialisation of the Sensors Network
 **/
void sensorsNetworkStart(){

  /* Create MessageQueue */
  mqSensorsSend = mq_open("/SensorsMsgSend", O_RDWR | O_CREAT, S_IRWXU, NULL);
  FAIL(mqSensorsSend);

  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(struct sockaddr_in));

  saddr.sin_addr.s_addr = inet_addr(conIP); 

  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(conPort);

  socklen_t len = sizeof(struct sockaddr_in);

  /* Create Socket */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  FAIL(sock);

  /* Connect Socket */
  FAIL(connect(sock, (struct sockaddr *)&saddr, len));

  puts("Connected to gateway");

  /* Launch Thread Reception */
  pthread_create(&pthreadSensorsRec, NULL, sensorsMsgRec, NULL);
  pthread_detach(pthreadSensorsRec);

  /* Launch Thread Sending */
  pthread_create(&pthreadSensorsSend, NULL, sensorsMsgSend, NULL);
  pthread_detach(pthreadSensorsSend);

}

/**
 * Desactivation of Sensors Network and free related memory
 **/
void sensorsNetworkStop(){

  FAIL(pthread_cancel(pthreadSensorsRec));
  FAIL(pthread_cancel(pthreadSensorsSend));
  FAIL(close(sock));
  FAIL(mq_close(mqSensorsSend));
  puts("Closed");
}

/**
 * Send Message toward Sensors
 **/
int sensorsNetworkSend(const char * msg_ptr, size_t msg_len){
  return mq_send( mqSensorsSend, msg_ptr, msg_len, 0);
}
