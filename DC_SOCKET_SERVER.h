#ifndef DC_SOCKET_SERVER
#define DC_SOCKET_SERVER

#include <netinet/in.h>

void bindSocket(struct sockaddr_in serverAddress, int socketFD);
void confirmClient(int establishedConnectionFD, char serverType);
struct sockaddr_in createServerAddress(int portNumber);
int createSocket();
int establishConnection(int socketFD);

#endif 