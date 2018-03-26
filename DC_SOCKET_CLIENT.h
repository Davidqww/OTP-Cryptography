#ifndef DC_SOCKET_CLIENT
#define DC_SOCKET_CLIENT

#include <netinet/in.h>

void checkFileLength(int plainTextLen, int keyLen, const char* keyFileName);
void checkServerType(int socketFD, int portNumber, char clientType);
void checkValidity(const char* plainText); 
void connectToServer(struct sockaddr_in serverAddress, int socketFD);
int createSocket();
int fileSize(char* fileName);
char* getFileContent(char* fileName);
struct sockaddr_in getServerAddress(int portNumber);

#endif