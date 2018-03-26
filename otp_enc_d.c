/************************************************************************************
This program is to be ran in the background as a daemon. Its function is to perform
actual encoding. This program will listen on a particular socket/port. When a 
connection is made, it will spawn a child process to handle the client requet. The 
child receives a plaintext and key via communication socket, and will write back the 
ciphertext.

USAGE: ./opt_enc_d <port> 
*************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "DC_SOCKET_SERVER.h"

/**********************************************************************
** char* encode(char*, char*)
** Generates a random key composed of upper case letters and the space
** character.
** 
** Args: (char*) text message to be encrypted
		 (char*) key used to encrypt text message
** Return: (char*) encoded string
**********************************************************************/
char* encode(char* plainText, char* key) {
	int holder;	// Holds the ASCII dec value when doing math expressions on chars 
	char* encodedText = malloc((strlen(plainText)) * sizeof(char));
	int i;
	for (i = 0; i < strlen(plainText); i++) {
		// Have space character be ASCII value 91 instead of 32 (this makes it so that A-Z + ' ' is 60 -91)
		if (plainText[i] == ' ') plainText[i] = 91;	
		if (key[i] == ' ') key[i] = 91;
		holder = (plainText[i] - 65) + (key[i] - 65);
		holder %= 27;
		holder += 65;
		if (holder == 91)
			encodedText[i] = ' ';
		else
			encodedText[i] = holder;
	}

	return encodedText;	
}

/********************************************************************
** void sendEncryptedMessage(int)
** Send client the encrypted message
** 
** Args: (int) socket file descriptor
** Return: none
********************************************************************/
void sendEncryptedMessage(int establishedConnectionFD) {
	char* key;	
	char* plainText;
	char* encodedText;	
	int msgLength;

	// Get the text length from client
	recv(establishedConnectionFD, &msgLength, sizeof(int), 0); 		

	// Receive encrypted text from client
	plainText = malloc((msgLength + 2) * sizeof(char));
	recv(establishedConnectionFD, plainText, msgLength, 0);

	// Receive key from client
	key = malloc((msgLength + 2) * sizeof(char));
    recv(establishedConnectionFD, key, msgLength, 0); 

	encodedText = encode(plainText, key);		           
	// Send back encrypted text
	send(establishedConnectionFD, encodedText, msgLength, 0); 
		
	free(key);
	free(plainText);
	free(encodedText);
}

int main(int argc, char *argv[])
{
	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	char serverType = 'e';	
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
			// Ensure client is trying to encode a message
			confirmClient(establishedConnectionFD, serverType);
			// Encode the message and send to client
			sendEncryptedMessage(establishedConnectionFD);
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
