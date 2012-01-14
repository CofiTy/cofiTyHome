#include sensorsNetwork.h
int sock;
pthread pthreadSensorsRec, pthreadSensorsSend;


void sensorsMsgRec(){
  
  char buff[128];
  int nb, total

    memset(buff, 0, 128);
  total = 0;

  for(;;)
  {
    /* reception form sensors */
    nb = recv(sock, buff, 32, 0);
    FAIL(nb)
      total += nb;

    /* If enough data we can process */
    if(total >= 104)
    {
      puts("recv");
      total = 0;
      memset(buff, 0, 32);
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

  pthread_create(&pthreadSensorsiSend, NULL, sensorsMsgSend, null);
  pthread_detach(pthreadSensorsSend);

}

void sensorsNetworkStop(){
  
  pthread_kill(&pthreadSensorsRec, SIGTERM);
  pthread_kill(&pthreadSensorsSend, SIGTERM);
  close(sock);
}
