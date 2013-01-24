/*****************************************************************************/
// Filename: TastySocket.h
/*****************************************************************************/
// Description: A light wrapper around a TCP socket. All it is really is a class
// with a socket fd as a member variable, and all the Berkely socket calls with the
// the first argument removed. Well...it's a little more than that. It's not meant
// to be used directly by anyone except for the other classes in this library...even
// though it could. However, it limits the full capabilities of TCP to give me a Facade
// for implementing the other classes in this library.
/*****************************************************************************/
#ifndef TASTY_SOCKET_H_DEFINED
#define TASTY_SOCKET_H_DEFINED

#include <cstdio>
#ifndef WIN32
#include <netdb.h>      /* hostent struct, gethostbyname() */
#include <arpa/inet.h>  /* inet_ntoa() to format IP address */
#include <netinet/in.h> /* in_addr structure */
#include <unistd.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <vector>
#include <string>
using std::vector;
using std::string;

#ifndef WIN32
typedef int socket_t;
#else
typedef SOCKET socket_t;
#endif

class CTastySocket
{
	public:
        //Asks the operating system for a file descriptor for TCP/IP socket via the socket() call. If a file descriptor can't be
        //allocated by OS, returns NULL.
		static CTastySocket* Create(string ipAddress, unsigned int port);

		//The destructor for this class
		~CTastySocket();

		//Server-specific calls
		//---------------------
		//Binds the socket to the ip address and port passed in the Create call via the system bind() call.
		int                  Bind();

		//Attempts to put the socket into listening mode via the system listen() call. Prerequisite is that the socket is bound. @see Bind()
		int                  Listen(int maxNumberOfClients);

		//If any clients are pending to connect, use this to accept them and talk to them. If no clients are pending, will return null.
		//The const MAXSIZE_OF_CLIENT_PENDING_QUEUE determines how many clients can connect. It is more efficient to use select() to
		//determine if clients are pending before doing this.
		CTastySocket*        Accept();

		//Client-specific calls
		//----------------------
		// Attempts to connect to server ip/port passed in the Create call via system connect() call.
		int                  Connect();

		//Common Calls
		//----------------------
		// Attempts to close the socket and relinquish file descriptor for reuse by OS via system close() call.
		int                  Close();

		// The IP Address passed in Create() call. For TastySockets created by Accept() call, the IP Address is the IP of the remote client
	  	string               IPAddress();

	  	// The Port passed in Create() call. For TastySockets created by Accept() call, this is the local port used to talk to the remote client.
		unsigned int         Port();

		// Receive data off TCP receive buffer by system recv() call. Returns num bytes read, 0 on disconnect, and -1 on error detailed by errno.
		int                  Receive(char* buffer, unsigned int numBytes);

		// Write data to TCP send buffer by system send() call. Returns num bytes sent, 0 on disconnect, and -1 on error detailed by errno.
		int                  Send(const char* buffer, unsigned int numBytes);

		// Simplified version of system select() call that does select on fd checking if OS will block on read requests. Returns 1 if
		// won't block, otherwise 0. On error -1 which can be checked by errno. Interpreted differently in different situations, for listening socket,
		// means clients are pending. For client or accept socket, means data is waiting on tcp receive buffer.
		int                  SelectRead(unsigned int timeoutInMicroseconds);

		// Simplified version of system select() call that does select on fd checking if OS will block on send requests. Returns 1 if
		// won't block, otherwise 0. On error -1 which can be checked by errno.
		int                  SelectWrite(unsigned int timeoutInMicroseconds);

		// Simplified version of system select() call that does select on fd TODO: Not sure what this is used for.
		int                  SelectExcept(unsigned int timeoutInMicroseconds);

		// Returns the file descriptor used by this socket.
		socket_t             SocketFD();

	private:
        //Constructor for this class.
        CTastySocket(socket_t fd, unsigned int port, string ipAddress);

        //Creates an interest set used by select, with only member being the SocketFD().
	    fd_set               FormFDSetForMe();

	    //Used to create a filled in sockaddr_in structure used by system calls, filling in stuff like protocols, ip, port, blah blah.
		sockaddr_in          FormSockAddrStructure();

		//Creates a timeval structure used by select() for specified timeout.
		timeval              FormTimevalStruct(unsigned int timeInMicroseconds);

		//Simplified version of select(), taking microseconds as unsigned int instead of a timeval struct.
		int                  Select(socket_t largestFD, fd_set* read, fd_set* write, fd_set* except, unsigned int timeoutInMicroseconds);

		//Sets the underlying file descriptor used by this socket.
		void                 SocketFD(socket_t fd);

	private:
		string               _ipAddress;
		unsigned int         _port;
		socket_t             _socketFD;
};

//Global calls to initialize the library
bool InitializeTheTastiness();
bool ShutdownTheTastiness();
#endif
