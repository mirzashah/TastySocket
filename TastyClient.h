/*****************************************************************************/
// Filename: TastyClient.h
/*****************************************************************************/
// Description: A TastyClient is the client version of a TastyStream.
/*****************************************************************************/

#ifndef TASTY_CLIENT_H_DEFINED
#define TASTY_CLIENT_H_DEFINED

#include "TastyStream.h"

class CTastyClient : public CTastyStream
{
    public:
        //Constructor for this class, takes ip and port of server client is to connect to.
        CTastyClient(string serverIPAddress, unsigned int serverPort);

        //Destructor for this class
        virtual ~CTastyClient();

        //Receive a tasty message @see::CTastyStream::Receive for more details.
        virtual TASTY_RC  Receive(TastyMessage& msg);

        //Returns the IP of the remote host the stream is connected to. Return empty string if currently unknown.
        virtual string    RemoteHostIP(){return(IPAddress());}

        //Returns the local port used to connect to the remote host. Return -1 if unknown.
        virtual int       RemoteHostPort(){return(Port());};

        //Sends a tasty message @see CTastyStream::Send for more details.
        virtual TASTY_RC  Send(TastyMessage& msg);

        //Closes the client using the close() call and destroys the low level socket. Also called automatically in destructor.
        virtual void      Close();

        //Returns true if client is connected to server, else false.
        virtual bool      IsConnected(){return(_isConnected);}

    private:
        //Ensures a connection to the server exists, if not, 1) makes sure a low level socket exists and 2)attempts to [re]connect to server.
        //@see CTastyStream::CreateLowLevelSocketIfNecessary()
        //@see EstablishConnectionToServerIfNecessary()
        TASTY_RC          EnsureConnectionToServerExists();

        //Establishes a connection to server if one does not exist.
        bool              EstablishConnectionToServerIfNecessary();

        //Setter for glad indicating if connected to server or not.
        void              IsConnected(bool isIt){_isConnected = isIt;}

    private:
        bool             _isConnected;
};

#endif
