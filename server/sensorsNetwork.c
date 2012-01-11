#include sensorsNetwork.h

int sensorsMsgRec(int sock)
{
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

int sensorsMsgSend(int sock);
{
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
