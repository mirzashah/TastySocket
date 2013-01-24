#include "TastyClient.h"
#include <cerrno>

/**************************************************************/
CTastyClient::CTastyClient(string serverIPAddress, unsigned int serverPort) :
CTastyStream(serverIPAddress, serverPort),
_isConnected(false)
/**************************************************************/
{
}

/**************************************************************/
CTastyClient::~CTastyClient()
/**************************************************************/
{
}

/**************************************************************/
void CTastyClient::Close()
/**************************************************************/
{
    CloseLowLevelSocket();
    IsConnected(false);
}

/**************************************************************/
TASTY_RC CTastyClient::EnsureConnectionToServerExists()
/**************************************************************/
{
    if(!CreateLowLevelSocketIfNecessary())
        return(TASTY_NOLOWLEVELSOCKET);

    if(!EstablishConnectionToServerIfNecessary())
        return(TASTY_NOCONNECTION);

    return(TASTY_OK);
}

/**************************************************************/
bool CTastyClient::EstablishConnectionToServerIfNecessary()
/**************************************************************/
{
    if(!IsConnected())
    {
        int rc = LowLevelSocket()->Connect();
        IsConnected(0 == rc);

        if(!IsConnected())
        {
#ifndef WIN32
            if((errno != EINPROGRESS)) 
#else
            if((errno != WSAEINPROGRESS))
#endif
                Close(); //Note: Can't reuse same low level FD if error code is not one of the ones in the above if condition
        }
    }

    return(IsConnected());
}

/**************************************************************/
TASTY_RC CTastyClient::Receive(TastyMessage& msg)
/**************************************************************/
{
    TASTY_RC connectRC = EnsureConnectionToServerExists();
    if(TASTY_OK != connectRC)
        return(connectRC);

    TASTY_RC rc = ReceiveCommon(*(LowLevelSocket()), msg);
    if(rc==TASTY_NOCONNECTION)
        IsConnected(false);
    return(rc);
}

/**************************************************************/
TASTY_RC CTastyClient::Send(TastyMessage& msg)
/**************************************************************/
{
    TASTY_RC connectRC = EnsureConnectionToServerExists();
    if(TASTY_OK != connectRC)
        return(connectRC);

    TASTY_RC rc = SendCommon(*(LowLevelSocket()), msg);
    if(rc==TASTY_NOCONNECTION)
        IsConnected(false);
    return(rc);
}
