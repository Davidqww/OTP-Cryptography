/*********************************************************************************
Client application that connects to otp_enc_d, and asks it to perform a one-time
pad style encryption. The encrypted message will be displayed onto shell terminal

USAGE: ./opt_enc <filename> <key_filename> <port> 
**********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "DC_SOCKET_CLIENT.h"

#define SLEEP_TIMER 50000	// Waiting time (in microseconds) between sending messages

/********************************************************************
** void encodeMessage(char*, char*, char*, int)
** Sends text length, text, and key to the server. Sever then sends
** back encrypted text.
**
** Args: (char*) plain text message to be encrypted
		 (char*) key to encode text message
		   (int) socket file descriptor
** Return: (char*) ecnText stores the encrypted text from the server
********************************************************************/
char* encodeMessage(char* plainText, char* key, int socketFD) {
	int textLen = strlen(plainText);
	char* encText = malloc(textLen * sizeof(char));

	// Send size of file to server
	send(socketFD, &textLen, sizeof(int), 0); 
	usleep(SLEEP_TIMER);

	// Send plainText to be encrypted
	send(socketFD, plainText, textLen * sizeof(char), 0); 
	usleep(SLEEP_TIMER);	

	// Send encryption key
	send(socketFD, key, textLen * sizeof(char), 0); // Send key
	usleep(SLEEP_TIMER);

	// Receive encrypted message from server and store in encText
	recv(socketFD, encText, textLen, 0); 

	return encText;
}
	 
int main(int argc, char *argv[])
{
	if (argc < 4) { fprintf(stderr,"USAGE: %s filename keyname port\n", argv[0]); exit(1); } // Check usage & args

	char* key;
	char* encText;
	char* plainText;
	char clientType;
	int portNumber, socketFD;
	struct sockaddr_in serverAddress;
	

	clientType = 'e';

	// Plain text from file to be encrypted
	plainText = getFileContent(argv[1]);

	// Key used to encypt file message
	key = getFileContent(argv[2]);

	// Convert portnumber from string to int
	portNumber = atoi(argv[3]); 

	// Ensure all characters in file are valid
	checkValidity(plainText);

	// Ensure the length key is long enough for file
	checkFileLength(strlen(plainText), strlen(key), argv[2]);	

	// Prepare server address 
	serverAddress = getServerAddress(portNumber);	

	// Set up the socket
	socketFD = createSocket();

	// Connect to server
	connectToServer(serverAddress, socketFD);

	// Check to see if connected to correct server
	checkServerType(socketFD, portNumber, clientType);
	
	// Get the encrypted message and store in encText
	encText = encodeMessage(plainText, key, socketFD);
	printf("%s\n", encText);

	// Close the socket
	close(socketFD); 

	// Deallocate memory 
	free(plainText);
	free(encText);
	free(key);

	return 0;
}
