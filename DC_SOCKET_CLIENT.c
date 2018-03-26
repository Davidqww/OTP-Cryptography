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

/*****************************************************************************
** void checkFileLength(int, int, const char*)
** Checks if key is long enough to encode message to be sent
** 
** Args: (int) length of text, (int) length of key, (char*) file name of key
** Return: none
******************************************************************************/
void checkFileLength(int plainTextLen, int keyLen, const char* keyFileName) {
	if (plainTextLen > keyLen) {	// Returns error if key length is shorter than text line
		fprintf(stderr, "Error: key '%s' is too short\n", keyFileName);
		exit(1);
	}
}

/********************************************************************
** void checkServerType(int, int, char)
** Contact server to see if the connection was made with the correct
** server
**
** Args: (int) socket file descriptor
		 (int) port number
		 (char) type of client 'e' encode or 'd' decode
** Return: none
********************************************************************/
void checkServerType(int socketFD, int portNumber, char clientType) {
	char response;	// Response given back to client, determining whether it is from otp_dec or otp_enc

	send(socketFD, &clientType, sizeof(char), 0);	// Send our client type to make sure we are contacting the correct server

	// Receive response from server
	recv(socketFD, &response, sizeof(char), 0);	
	if (response != 'y') {	// If server did not reply with 'y', then we contacted wrong server
		close(socketFD);
		fprintf(stderr, "Error: could not contact otp_enc_d on port %d\n", portNumber);
                exit(2);
	}	
}

/******************************************************************
** void checkValidity(const char*)
** Checks if file contents contain invalid characters
** 
** Args: (char*) content in file
** Return: none
******************************************************************/
void checkValidity(const char* plainText) {
	int i;
	for (i = 0; i < strlen(plainText); i++) {	// Make sure all characters in file are spaces and Upper Case letters
		if(!(plainText[i] == ' ' || (plainText[i] >= 'A' && plainText[i] <= 'Z'))) {
			fprintf(stderr, "otp_enc error: input contains bad characters\n");
			exit(1);
		}
	}
}

/*****************************************************************
** void connectToServer(struct sockaddr_in, int)
** Connect to the server
**
** Args: (sockaddr_in)address info, (int)socket file descriptor
** Return: none
*****************************************************************/
void connectToServer(struct sockaddr_in serverAddress, int socketFD) {
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){ 
		fprintf(stderr, "CLIENT: ERROR connecting");
		exit(1);
	}
}

/*****************************************************************
** int createSocket()
** Creates TCP socket
** 
** Args: none
** Return: (int)socket file descriptor
*****************************************************************/
int createSocket() {
	// TCP socket
	int socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0) {
		fprintf(stderr, "CLIENT: ERROR, cannot create socket");
		exit(1);
	}
	return socketFD;
}


/*****************************************************************
** int fileSize(char* )
** Get the size(in bytes) of a file
** 
** Args: (char*) file name
** Return: (int)size of file in bytes
*****************************************************************/
int fileSize(char* fileName) {
	struct stat st;
	stat(fileName, &st);
	return st.st_size;
}

/******************************************************************
** char* getFileContent(char* )
** Opens a file and gets the first line
** 
** Args: (char*) file name
** Return: (char*) string containing first line in sepcified file
******************************************************************/
char* getFileContent(char* fileName) {
	FILE* file = fopen(fileName, "r");	// Open file with matching filename
	if (file == NULL) {	// Error opening the file
		fprintf(stderr, "Error opening file: %s\n", fileName);
		exit(1);
	}
	char* line;	// Holds the line that is in the file
	int fileLength = fileSize(fileName);	// Get the size of the line
	line = malloc((fileLength) * sizeof(char));
	if (fgets(line, fileLength, file) == NULL) {	// Error getting file
		fprintf(stderr, "Error getting line from file: %s\n", fileName);
		exit(1);
	}
	fclose(file);	// Close file
	
	return line;	
}

/*****************************************************************
** struct sockaaddr_in getServerAddress(char*, int)
**
** Set-up the address and port number 
** Args: (char)host name, (int) port number 
** Return: the address info 
*****************************************************************/
struct sockaddr_in getServerAddress(int portNumber) {
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	serverHostInfo = gethostbyname("localhost");  
	if (serverHostInfo == NULL) {
		 fprintf(stderr, "CLIENT: ERROR, no such host\n");
		 exit(0);
	}

	// Clear out address struct before use	
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));
	// IPv4
	serverAddress.sin_family = AF_INET;
	// Convert port number to network byte order 
	serverAddress.sin_port = htons(portNumber); 
	
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

	return serverAddress;
}