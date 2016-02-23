/*
 * udp-directory-server.c
 *
 *  Created on: Feb 23, 2016
 *      Author: Autumn K. Conner, Alisha Geis
 */

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define ECHOMAX 255     /* Longest string to echo */

void DieWithError(char *errorMessage);  /* External error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */

    struct message{					 /* Struct to account for incoming messages from clients */
    	int messageType;
    	char hostName[24];
    	char address[24];
    	char fileName[50];
    	long fileSize;
    };


    if (argc != 2)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);

        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
            (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

	char responseBuffer[128];
	char *mess = echoBuffer;	//pointer to buffer to receive message in proper format

    int i = 0;
    int t;
    long x;
    char current[24];

    struct message messArray[200];	//array of message structs for directory server to store

	if (echoBuffer[0] == '1') {

		mess = strtok(mess, "/");
		//messArray[i].messageType = &mess;

		mess = strtok(mess, "/");
		strcpy(messArray[i].hostName, &mess);

		mess = strtok(mess, "/");
		strcpy(messArray[i].address, &mess);

		mess = strtok(mess, "/");
		strcpy(messArray[i].fileName, &mess);

		mess = strtok(mess, "\0");
		//messArray[i].fileSize = x;

		strcpy(responseBuffer, "Inform and update received!");

		current = messArray[i].hostName;
		strcat(responseBuffer, current);

		i++;
	}
	else if(echoBuffer[0] == '2') {
		strcpy(responseBuffer, "I have these files:\nabc.txt 10.0.0.0 25500 bytes");
	}
	else {
		strcpy(responseBuffer, "Invalid command received");
	}

        /* Send response back to the client */
	int responseBufferSize = strlen(responseBuffer);
        if (sendto(sock, responseBuffer, responseBufferSize, 0,
             (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != responseBufferSize)
            DieWithError("sendto() sent a different number of bytes than expected");
    }
    /* NOT REACHED */
}
