#include "TastyServer.h"
#include <cassert>
#include <iostream>
using namespace std;

/*****************************************************/
CTastyServer::CTastyServer(string listenIPAddress, unsigned int listenPort) :
CTastyStream(listenIPAddress, listenPort),
_acceptedClient(NULL),
_isListeningSocketBound(false),
_isListeningSocketListening(false)
/*****************************************************/
{
}

/*****************************************************/
CTastyServer::~CTastyServer()
/*****************************************************/
{
    Close();
}

/*****************************************************/
CTastySocket* CTastyServer::AcceptedClient()
/*****************************************************/
{
    return(_acceptedClient);
}

/*****************************************************/
void CTastyServer::AcceptedClient(CTastySocket* sock)
/*****************************************************/
{
    if(AcceptedClient()!=NULL) //Destroy accepted client
    {
        AcceptedClient()->Close();
        delete(AcceptedClient());
    }
    _acceptedClient = sock;
}

/*****************************************************/
bool CTastyServer::AreClientsPendingForAcception()
/*****************************************************/
{
    int numClientsWaiting = LowLevelSocket()->SelectRead(TimeoutInMicroseconds());
    return(1==numClientsWaiting);
}

/*****************************************************/
void CTastyServer::Close()
/*****************************************************/
{
    AcceptedClient(NULL); //Destroy accepted client if it exists
    CloseLowLevelSocket(); //Destroy listening socket
    IsListeningSocketBound(false);
    IsListeningSocketListening(false);
}

/*****************************************************/
TASTY_RC CTastyServer::EnsureConnectionToClientExists()
/*****************************************************/
{
    TASTY_RC initRC = InitializeServerIfNecessary();
    if(TASTY_OK != initRC)
        return(initRC);

    if(!(MakeSureAcceptedClientExists()))
        return(TASTY_NOCONNECTION);

    return(TASTY_OK);
}

/*****************************************************/
TASTY_RC CTastyServer::InitializeServerIfNecessary()
/*****************************************************/
{
    if(!CreateLowLevelSocketIfNecessary())
        return(TASTY_NOLOWLEVELSOCKET);

    if(!IsListeningSocketBound())
    {
        IsListeningSocketBound(0 == LowLevelSocket()->Bind());
        if(!IsListeningSocketBound())
        {
            Close();  //Note: I found that I can't reuse the same FD if bind fails, need to close it and create a new low level socket
            return(TASTY_SERVERNOTBOUND);
        }
    }
    if(!IsListeningSocketListening())
    {
        IsListeningSocketListening(0 == LowLevelSocket()->Listen(1)); //Listen takes 1 as only one client can be connected
        if(!IsListeningSocketListening())
            return(TASTY_SERVERNOTLISTENING);
    }
    return(TASTY_OK);
}

/*****************************************************/
bool CTastyServer::IsConnected()
/*****************************************************/
{
    return(NULL != AcceptedClient());
}

/*****************************************************/
bool CTastyServer::MakeSureAcceptedClientExists()
/*****************************************************/
{
    if(AcceptedClient()==NULL)
    {
        if(!AreClientsPendingForAcception())
            return(false);

        AcceptedClient(LowLevelSocket()->Accept());
        return(NULL != AcceptedClient());
    }
    else
        return(true);
}

/*****************************************************/
TASTY_RC CTastyServer::Receive(TastyMessage& msg)
/*****************************************************/
{
    TASTY_RC connectRC = EnsureConnectionToClientExists();
    if(TASTY_OK != connectRC)
        return(connectRC);

    assert(AcceptedClient()!=NULL);
    TASTY_RC recvRC = ReceiveCommon(*(AcceptedClient()), msg);
    if(TASTY_NOCONNECTION == recvRC)
        AcceptedClient(NULL);
    return(recvRC);
}

/*****************************************************/
string CTastyServer::RemoteHostIP()
/*****************************************************/
{
    if(NULL!=AcceptedClient())
        return(AcceptedClient()->IPAddress());
    else
        return("");
}

/*****************************************************/
int CTastyServer::RemoteHostPort()
/*****************************************************/
{
    if(NULL!=AcceptedClient())
        return(AcceptedClient()->Port());
    else
        return(-1);
}

/*****************************************************/
TASTY_RC CTastyServer::Send(TastyMessage& msg)
/*****************************************************/
{
    TASTY_RC connectRC = EnsureConnectionToClientExists();
    if(TASTY_OK != connectRC)
        return(connectRC);

    assert(AcceptedClient()!=NULL);
    TASTY_RC sendRC = SendCommon(*(AcceptedClient()), msg);
    if(TASTY_NOCONNECTION == sendRC)
        AcceptedClient(NULL);
    return(sendRC);
}

