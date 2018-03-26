/*********************************************************************************
This program is to be ran in the background as a daemon. Its function is to perform
actual decoding. This program will listen on a particular socket/port. When a 
connection is made, it will spawn a child process to handle the client requet. The 
child receives an encrypted text and key via communication socket, and will write 
back the decyphered text

USAGE: ./opt_enc_d <port> 
**********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "DC_SOCKET_SERVER.h"

// returns decoded text
char* decode(char* cypherText, char* key) {
	int holder;	// Holds the ASCII dec value when doing math expressions on chars 
	char* decodedText = malloc((strlen(cypherText)) * sizeof(char));
	int i;
	for (i = 0; i < strlen(cypherText); i++) {
		if (cypherText[i] == ' ') cypherText[i] = 91;	// Have space be a value of 91 instead of 32 (this makes it so that A-Z + ' ' is 60 -91)
		if (key[i] == ' ') key[i] = 91;
		holder = cypherText[i] - key[i];	
		if (holder < 0)		// It seems like we cannot modulo a negative number? Had to add 27 instead to get modulo
			holder += 27;
		else
			holder %= 27;
		holder += 65;
		if (holder == 91)
			decodedText[i] = ' ';
		else
			decodedText[i] = holder;
	}
	return decodedText;	// return decoded text
}

/********************************************************************
** void sendDecryptedMessage(int)
** Send client the decrypted message
** 
** Args: (int) socket file descriptor
** Return: none
********************************************************************/
void sendDecryptedMessage(int establishedConnectionFD) {
	char* key;	
	char* encodedText;
	char* decodedText;
	int msgLength;

	// Get the text length from client
	recv(establishedConnectionFD, &msgLength, sizeof(int), 0); 		

	// Receive encrypted text from client
	encodedText = malloc((msgLength + 2) * sizeof(char));
	recv(establishedConnectionFD, encodedText, msgLength, 0); 	

	// Receive key from client
	key = malloc((msgLength + 2) * sizeof(char));
    recv(establishedConnectionFD, key, msgLength, 0 + 2);

	decodedText = decode(encodedText, key);		           
	// Send back decrypted text
	send(establishedConnectionFD, decodedText, msgLength, 0); 
		
	free(key);
	free(encodedText);
	free(decodedText);
}

int main(int argc, char *argv[])
{
	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	char serverType = 'd';	
	struct sockaddr_in serverAddress;
	int listenSocketFD, establishedConnectionFD, portNumber;

	// Convert portnumber from string to int
	portNumber = atoi(argv[1]);

	// Create address of server
	serverAddress = createServerAddress(portNumber);

	// Create socket
	listenSocketFD = createSocket();

	// Bind the socket to server address
	bindSocket(serverAddress, listenSocketFD);

	// Flip the socket on - it can now receive up to 5 connections
	listen(listenSocketFD, 5); 

	while(1) {
		// Establish any incoming connections
		establishedConnectionFD = establishConnection(listenSocketFD);
		
		// Create a child process to handle client requests
		pid_t pid = fork();

		if (pid < 0) {
			fprintf(stderr, "Problem creating child\n");
			exit(1);
		} 

		else if (pid ==0) {	// Child
			// Ensure client is trying to decode a message
			confirmClient(establishedConnectionFD, serverType);
			// Decode the message and send to client
			sendDecryptedMessage(establishedConnectionFD);
			// Close the descriptor
			shutdown(establishedConnectionFD, 2);
			// Kill child process
			exit(0);
		}
	}

	// Close the listening socket
	close(listenSocketFD); 

	return 0; 
}
