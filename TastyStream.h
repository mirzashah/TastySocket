/*****************************************************************************/
// Filename: TastyStream.h
/*****************************************************************************/
// Description: This class defines an abstract base class for a
// TCP data stream.The two subclasses TastyClient and TastyServer override
// the interface specified by this class in order to provide a way to create
// a one-to-one data pipe within/between applications. The point is to ignore
// the server/client paradigm...whereever a TastyStream is used, it can be
// changed transparently to a TastyServer or TastyClient without affecting
// program behavior. The concept is identical to Matt Davis' TCPCommunicator,
// this is a less ambitious and lesser complicated clone of the same idea.
/*****************************************************************************/

#ifndef TASTY_STREAM_H_DEFINED
#define TASTY_STREAM_H_DEFINED

#include "TastySocket.h"
#include "TastyMessage.h"

enum TASTY_RC
{
    TASTY_OK,                  //Call successfully completed
    TASTY_PENDING,             //A TastyMessage that was attempted to be sent or received could only partially be completed. Try sending/receiving again later to get the rest through.
    TASTY_NOLOWLEVELSOCKET,    //The OS could not allocated a file descriptor for the stream to utilize
    TASTY_NOCONNECTION,        //No connection exists right now, either in the form of a connection to the server (CLIENT) or an accepted client (SERVER)
    TASTY_NODATAONBUFFER,      //No data to receive on the tcp receive buffer currently.
    TASTY_SERVERNOTBOUND,      //(ONLY FOR SERVER) The listening socket is not bound
    TASTY_SERVERNOTLISTENING,  //(ONLY FOR SERVER) The listening socket is not in a listening state
    TASTY_ERROR                //Some other unexpected socket error. Check errno for more details.
};

class CTastyStream
{
    public:
        virtual ~CTastyStream();

        //Shutsdown the stream, closing any file descriptors and resetting state. Even after this method is invoked,
        //a subclass should attempt to reestablish the link if a user utilizes the Receive and Send calls (i.e. the object
        //is reusable even after this method is invoked).
        virtual void         Close() = 0;

        //Indicates if a tcp session is established to the best of knowledge of the OS.
        virtual bool         IsConnected() = 0;

        //Attempts to receive a TastyMessage. Note that an entire message might not make it through in a single receive call,
        //as indicated by return call.
        //see enum TASTY_RC
        virtual TASTY_RC     Receive(TastyMessage& msg) = 0;

        //Returns the IP of the remote host the stream is connected to. Return empty string if currently unknown.
        virtual string       RemoteHostIP() = 0;

        //Returns the local port used to connect to the remote host. Return -1 if unknown.
        virtual int          RemoteHostPort() = 0;

        //Attempts to send a TastyMessage. Note that on success (TASTY_OK), does not guarantee the message made it to the remote host,
        //but rather the data was written to the OS' tcp send buffer. If there is not enough room on the buffer, the entire message
        //may not be written.
        //see enum TASTY_RC
        virtual TASTY_RC     Send(TastyMessage& msg) = 0;

        //Sets the timeout used for select() calls, zero by default
        void                 TimeoutInMicroseconds(int usec){_timeoutInMicroseconds = usec;}

    protected:
        CTastyStream(string ipAddress, unsigned int port);
        bool                 CreateLowLevelSocketIfNecessary();
        void                 CloseLowLevelSocket();
        string               IPAddress(){return(_ipAddress);}
        CTastySocket*        LowLevelSocket(){return(_lowLevelSocket);}
        unsigned int         Port(){return(_port);}
        TASTY_RC             ReceiveCommon(CTastySocket& sock, TastyMessage& msg);
        TASTY_RC             SendCommon(CTastySocket& sock, TastyMessage& msg);
        int                  TimeoutInMicroseconds(){return(_timeoutInMicroseconds);}

    private:
        void                 LowLevelSocket(CTastySocket* s){_lowLevelSocket = s;}

    private:
        string               _ipAddress;
        CTastySocket*        _lowLevelSocket;
        unsigned int         _port;
        int                  _timeoutInMicroseconds;
};

#endif
