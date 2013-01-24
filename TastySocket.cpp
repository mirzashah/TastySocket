#include "TastySocket.h"
#include <cassert>
#include <cstring>
#include <signal.h>

/******************************************************/
CTastySocket::CTastySocket(socket_t fd, unsigned int clientPort, string clientIPAddress) :
_ipAddress(clientIPAddress),
_port(clientPort),
_socketFD(fd)
/******************************************************/
{
}

/******************************************************/
CTastySocket::~CTastySocket()
/******************************************************/
{
}

/******************************************************/
CTastySocket* CTastySocket::Accept()
/******************************************************/
{
#ifdef WIN32
#define TASTY_SIZE_TYPE int
#else
#define TASTY_SIZE_TYPE unsigned int
#endif

    sockaddr_in  clientSockAddrInfo;
	TASTY_SIZE_TYPE clientSockAddrInfoSize = sizeof(clientSockAddrInfo);
	socket_t newSocket = accept(SocketFD(), (sockaddr*)(&clientSockAddrInfo), &(clientSockAddrInfoSize));

#ifndef WIN32
	if(newSocket < 0)
#else
    if(newSocket == INVALID_SOCKET)
#endif
		return(NULL);
	else
	{
		string clientIPAddress(inet_ntoa(clientSockAddrInfo.sin_addr));
		int    clientPort = clientSockAddrInfo.sin_port;

		return(new CTastySocket(newSocket, clientPort, clientIPAddress));
	}
}

/******************************************************/
int CTastySocket::Bind()
/******************************************************/
{
	sockaddr_in sockAddrInfo = FormSockAddrStructure();
	return(bind(SocketFD(), (sockaddr*)(&sockAddrInfo), sizeof(sockAddrInfo)));
}

/******************************************************/
int CTastySocket::Close()
/******************************************************/
{
#ifndef WIN32
	return(close(SocketFD()));
#else
    return(closesocket(SocketFD()));
#endif
}

/******************************************************/
int CTastySocket::Connect()
/******************************************************/
{
	sockaddr_in sockAddrInfo = FormSockAddrStructure();
	return(connect(SocketFD(), (sockaddr*)(&sockAddrInfo), sizeof(sockAddrInfo)));
}

/******************************************************/
CTastySocket* CTastySocket::Create(string ipAddress, unsigned int port)
/******************************************************/
{
    socket_t fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(fd>0)
    {
        int on = 1;
#ifndef WIN32
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#else
        BOOL bOptVal = TRUE;
        int bOptLen = sizeof(BOOL);
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&bOptVal, bOptLen);
#endif
        return(new CTastySocket(fd, port, ipAddress));
    }
    else
        return(NULL);
}

/************************************************************/
fd_set CTastySocket::FormFDSetForMe()
/************************************************************/
{
	fd_set set;
	FD_ZERO(&set);

	FD_SET(SocketFD(), &set);

	return(set);
}

/******************************************************/
sockaddr_in CTastySocket::FormSockAddrStructure()
/******************************************************/
{
	sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family      = AF_INET;
	sa.sin_addr.s_addr = inet_addr(IPAddress().c_str());
	sa.sin_port        = htons(Port());

	return(sa);
}

/************************************************************/
timeval CTastySocket::FormTimevalStruct(unsigned int timeInMicroseconds)
/************************************************************/
{
	timeval toReturn;
	toReturn.tv_sec = 0;
	toReturn.tv_usec = timeInMicroseconds;
	return(toReturn);
}

/******************************************************/
string CTastySocket::IPAddress()
/******************************************************/
{
	return(_ipAddress);
}

/******************************************************/
int CTastySocket::Listen(int maxNumberOfClients)
/******************************************************/
{
    if(maxNumberOfClients < 1)
        maxNumberOfClients = 1;
	return(listen(SocketFD(), maxNumberOfClients));
}

/******************************************************/
int CTastySocket::Receive(char* buffer, unsigned int numBytes)
/******************************************************/
{
#ifndef WIN32
	return(recv(SocketFD(), buffer, numBytes, MSG_DONTWAIT));
#else
    return(recv(SocketFD(), buffer, numBytes, 0));
#endif
}

/******************************************************/
unsigned int CTastySocket::Port()
/******************************************************/
{
	return(_port);
}

/************************************************************/
int CTastySocket::Select(socket_t largestFD, fd_set* read, fd_set* write, fd_set* except, unsigned int timeInMicroseconds)
/************************************************************/
{
	timeval tm = FormTimevalStruct(timeInMicroseconds);
	return(select(largestFD+1, read, write, except, &(tm)));
}

/************************************************************/
int CTastySocket::SelectRead(unsigned int timeoutInMicroseconds)
/************************************************************/
{
    fd_set readSet = FormFDSetForMe();
    return(Select(SocketFD(), &(readSet), NULL, NULL, timeoutInMicroseconds));
}

/************************************************************/
int CTastySocket::SelectWrite(unsigned int timeoutInMicroseconds)
/************************************************************/
{
    fd_set writeSet = FormFDSetForMe();
    return(Select(SocketFD(), NULL, &(writeSet), NULL, timeoutInMicroseconds));
}

/************************************************************/
int CTastySocket::SelectExcept(unsigned int timeoutInMicroseconds)
/************************************************************/
{
    fd_set exceptSet = FormFDSetForMe();
    return(Select(SocketFD(), NULL, NULL, &(exceptSet), timeoutInMicroseconds));
}

/******************************************************/
int CTastySocket::Send(const char* buffer, unsigned int numBytes)
/******************************************************/
{
#ifndef WIN32
	return(send(SocketFD(), buffer, numBytes, MSG_DONTWAIT));
#else
    return(send(SocketFD(), buffer, numBytes, 0));
#endif
}

/******************************************************/
socket_t CTastySocket::SocketFD()
/******************************************************/
{
	return(_socketFD);
}

/******************************************************/
void CTastySocket::SocketFD(socket_t fd)
/******************************************************/
{
	_socketFD = fd;
}

/******************************************************/
/******************************************************/
/******************************************************/
/******************************************************/
/******************************************************/

//Global calls below

/***********************************************************/
bool InitializeTheTastiness()
/***********************************************************/
{
#ifndef WIN32
    sigignore(SIGPIPE); //In Linux, it is possible to get a broken pipe signal on a socket before writing is finished. This ignores the signal...error codes within TastySocket handle...
#else
  // Initialize Winsock
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  return(iResult == NO_ERROR);
#endif
}

/***********************************************************/
bool ShutdownTheTastiness()
/***********************************************************/
{
#ifdef WIN32
    WSACleanup();
#endif
    return(true);
}



