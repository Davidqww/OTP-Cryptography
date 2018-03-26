#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "DC_SOCKET_SERVER.h"

/*************************************************************************
** int bindSocket()
** Binds socket with address
** 
** Args: (struct sockaddr_in) server address, (int) socket file desciptor
** Return: (int) socket file descriptor
**************************************************************************/
void bindSocket(struct sockaddr_in serverAddress, int socketFD) {
	if (bind(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
		fprintf(stderr, "SERVER: ERROR on binding. Try another port number.\n");
		exit(1);
	}
}

/********************************************************************
** void confirmClient(int)
** Ensure client request is valid
** 
** Args: (int) socket file descriptor
** Return: none
********************************************************************/
void confirmClient(int establishedConnectionFD, char serverType) {
	char incomingClient, response;

	recv(establishedConnectionFD, &incomingClient, sizeof(int), 0);

	if (incomingClient == serverType) {	// If client wants encypting, they they contacted the correct server
		response = 'y';	// Reply back to client that this is the correct server
		send(establishedConnectionFD, &response, sizeof(char), 0);
	}
	else {	// Not the correct client
		response = 'n';
		send(establishedConnectionFD, &response, sizeof(char), 0);
		close(establishedConnectionFD); // Close the existing socket which is connected to the client
		exit(1);
	}
}

/********************************************************************
** struct sockaddr_in createServerAddress(int)
** Create the server address
** 
** Args: (int)port number
** Return: (struct sockaddr_in) server address
********************************************************************/
struct sockaddr_in createServerAddress(int portNumber) {
	struct sockaddr_in serverAddress;
	// Clear out address struct before use
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));
	// IPv4
	serverAddress.sin_family = AF_INET;
	// Convert port number to network byte order
	serverAddress.sin_port = htons(portNumber);
	// Any address is allowed for connection to this process
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	
	return serverAddress;
}

/*********************************************************************
** int createSocket()
** Create a socket
** 
** Args: none
** Return: (int) socket file descriptor
**********************************************************************/
int createSocket() {
	// TCP socket
	int socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0) {
		fprintf(stderr, "SERVER: Error, cannot create socket\n");
		exit(1);
	}
	return socketFD;
}

/********************************************************************
** int establishConnection(int)
** Establish connection with client
** 
** Args: (int) socket file descriptor
** Return: (int)file descriptor of established connection
********************************************************************/
int establishConnection(int socketFD) {
	struct sockaddr_in clientAddress;
	socklen_t sizeOfClientInfo;
    int establishedConnectionFD;
    char ipAddr[INET_ADDRSTRLEN];

	sizeOfClientInfo = sizeof(clientAddress);
	// Accept connection		
	establishedConnectionFD = accept(socketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
	if (establishedConnectionFD < 0) 
			fprintf(stderr, "ERROR on accept\n");

	// Print out the IP address of connected client
	inet_ntop(AF_INET, &(clientAddress.sin_addr), ipAddr, INET_ADDRSTRLEN);
	printf("Connection established with %s\n", ipAddr);

	return establishedConnectionFD;
}