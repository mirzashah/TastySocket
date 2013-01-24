/*****************************************************************************/
// Filename: TastyServer.h
/*****************************************************************************/
// Description: A TastyServer is the server version of a TastyStream. TastyServer
// is not meant to be a full-fledges TCP server that accepts and manages multiple
// clients, rather it just manages one client. The TastyServer is meant to behave
// in the same way as a TCP client by hiding the fact that there are two sockets
// under the hood, one for listening and the accepted client. This makes it easy
// to swap in a TasyServer with a TastyClient and vice-versa wherever a TastyStream
// is expected.
/*****************************************************************************/

#ifndef TASTY_SERVER_H_DEFINED
#define TASTY_SERVER_H_DEFINED

#include "TastyStream.h"

class CTastyServer : public CTastyStream
{
    public:
        //Constructor for this class. Specifies the ip and port server will be bound and listening on.
        CTastyServer(string listenIPAddress, unsigned int listenPort);

        //Class destructor
        virtual ~CTastyServer();

        //Shuts down the server by 1) closing the accepted client if it exists and 2) closing the listening socket and resetting all
        //internal state. This is also called automatically by destructor.
        virtual void     Close();

        //Users don't need to call this, but if they want to explicitly get the server bound and listening,
        //before doing Send and Receives, they can do so with this call
        TASTY_RC         InitializeServerIfNecessary();

        //This indicates if the server has a client connected to it.
        virtual bool     IsConnected();

        //Receives a tasty message @see CTastyStream::Receive for more details.
        virtual TASTY_RC Receive(TastyMessage& msg);

        //Returns the IP of the remote host the stream is connected to. Return empty string if currently unknown.
        virtual string       RemoteHostIP();

        //Returns the local port used to connect to the remote host. Return -1 if unknown.
        virtual int          RemoteHostPort();

        //Sends a tasty message @see CTastyStream::Send for more details.
        virtual TASTY_RC Send(TastyMessage& msg);

    private:
        //A handle to the accepted client. Returns null if no client is connected. TastyServer limits one connected client as is the
        //point of TastyServer to hide listening and accept socket.
        CTastySocket*    AcceptedClient();

        //Sets handle to accepted client. If a client already exists, will close and destroy that socket before setting it to the new value.
        void             AcceptedClient(CTastySocket* sock);

        //Uses select() to check if a client is waitnig to be accepted.
        bool             AreClientsPendingForAcception();

        //Attempts to check if a connection to client exists. If not, will attempt to create one by 1) making sure low level
        //listening socket exists, 2) making sure listening socket is intialized and 3) making accepting a pending client
        //@see CTastyStream::CreateLowLevelSocketIfNecessary
        //@see InitializeServerIfNecessary
        //@see MakeSureAcceptedClientExists
        TASTY_RC         EnsureConnectionToClientExists();

        //Gets if the listening socket @see LowLevelSocket() is bound
        bool             IsListeningSocketBound(){return(_isListeningSocketBound);}

        //Sets if the listening socket @see LowLevelSocket() is bound
        void             IsListeningSocketBound(bool isIt){_isListeningSocketBound = isIt;}

        //Gets if the listening socket @see LowLevelSocket() is listening
        bool             IsListeningSocketListening(){return(_isListeningSocketListening);}

        //Sets if the listening socket @see LowLevelSocket() is listening
        void             IsListeningSocketListening(bool isIt){_isListeningSocketListening = isIt;}

        //Ensures a connection to a client exists, if not, will attempt to do so via accept() call
        bool             MakeSureAcceptedClientExists();

    private:
        CTastySocket*    _acceptedClient;
        bool             _isListeningSocketBound;
        bool             _isListeningSocketListening;

};


#endif
