#include <pthread.h>
#include <mqueue.h>
#include "sensorsNetwork.h"

int sock;
pthread pthreadSensorsRec, pthreadSensorsSend;
mqd_t mqSensorsSend;

void sensorsMsgRec(){

  char buff[128];
  int nb, total
    char* receiving = (char *) buff[0];

  memset(buff, 0, 128);
  total = 0;

  for(;;)
  {
    /* reception form sensors */
    nb = recv(sock, buff, 128, 0);
    FAIL(nb)

      total += nb;
    receiving += nb;

    /* If enough data we can process */
    if(total >= 104)
    {
      puts("recv");
      total = 0;
      receiving = (char *) buff[0];
      memset(buff, 0, 128);
    }
  }
}

void sensorsMsgSend();{

  char buff[128];
  int nb, nbSent, total;
  char* sending = (char *) buff[0];

  for(;;)
  {
    /* Recuperation des messages de la boite au lettre "Envoi" */
    nb = msgQReceive(balEnvoi, buff, SIZE_BUFF, WAIT_FOREVER);
    nb= mq_receive(mqSensorsSend, buff, 128, 0);
    FAIL(nb)

      total = nb;
    nbSent = 0;
    while(nbSent < total)
    {
      /* Sending toward sensors */
      nb = send(sock, sending, nb, 0);
      FAIL(nb)
        nbSent += nb;
      sending += nb;
    }
    puts("sent");
  }

}

void sensorsNetworkStart(){

  mqSensorsSend = mq_open("SensorsMsgSend", O_RDWR | O_CREAT);

  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(struct sockaddr_in));

  saddr.sin_addr.s_addr = htonl(134.214.1.28); //TODO macro avec les bonnes valeur
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(5000); //TODO macro avec les bonnes valeur

  sock = socket(AF_INET, SOCK_STREAM, 0);
  FAIL(sock);

  FAIL(connect(sock, (struct sockaddr *)&saddr, len));

  pthread_create(&pthreadSensorsRec, NULL, sensorsMsgRec, null);
  pthread_detach(pthreadSensorsRec);

  pthread_create(&pthreadSensorsSend, NULL, sensorsMsgSend, null);
  pthread_detach(pthreadSensorsSend);

}

void sensorsNetworkStop(){

  pthread_kill(&pthreadSensorsRec, SIGTERM);
  pthread_kill(&pthreadSensorsSend, SIGTERM);
  close(sock);
}

int sensorsNetworkSend(const char * msg_ptr, size_t msg_len){
  return mq_send( mqSensorsSend, msg_ptr, msg_len, 0);
}
