#include "TastyStream.h"

/************************************************************/
CTastyStream::CTastyStream(string ipAddress, unsigned int port) :
_ipAddress(ipAddress),
_lowLevelSocket(NULL),
_port(port),
_timeoutInMicroseconds(0)
/************************************************************/
{
}

/************************************************************/
CTastyStream::~CTastyStream()
/************************************************************/
{
}

/**************************************************************/
bool CTastyStream::CreateLowLevelSocketIfNecessary()
/**************************************************************/
{
    if(NULL==LowLevelSocket())
    {
        LowLevelSocket(CTastySocket::Create(IPAddress(), Port()));
    }
    return(NULL!=LowLevelSocket());
}

/**************************************************************/
void CTastyStream::CloseLowLevelSocket()
/**************************************************************/
{
    if(NULL!=LowLevelSocket())
    {
        LowLevelSocket()->Close();
        delete(LowLevelSocket());
        LowLevelSocket(NULL);
    }
}

/**************************************************************/
TASTY_RC CTastyStream::ReceiveCommon(CTastySocket& sock, TastyMessage& msg)
/**************************************************************/
{
    if(1 != sock.SelectRead(TimeoutInMicroseconds())) //If no data available, no point calling recv
        return(TASTY_NODATAONBUFFER);

    if(msg._bytesRemaining==0)
        return(TASTY_OK); //Message already received

    int bytesReceived = sock.Receive(&(msg._buffer[msg._messageSize - msg._bytesRemaining]), msg._bytesRemaining);
    if(bytesReceived>0)
    {
        msg._bytesRemaining = msg._bytesRemaining - bytesReceived;
        if(msg._bytesRemaining==0)
            return(TASTY_OK);
        else if(msg._bytesRemaining>0)
            return(TASTY_PENDING);
        else
            throw; //shouldn't happen, something wrong with internal counting
    }
    else if(bytesReceived==0) //Connection shutdown
        return(TASTY_NOCONNECTION);
    else
        return(TASTY_ERROR); //Check errno, something else went wrong
}

/**************************************************************/
TASTY_RC CTastyStream::SendCommon(CTastySocket& sock, TastyMessage& msg)
/**************************************************************/
{
    if(msg._bytesRemaining==0)
        return(TASTY_OK); //Message already sent

    int bytesSent = sock.Send(&(msg._buffer[msg._messageSize - msg._bytesRemaining]), msg._bytesRemaining);
    if(bytesSent>0)
    {
        msg._bytesRemaining = msg._bytesRemaining - bytesSent;
        if(msg._bytesRemaining==0)
            return(TASTY_OK);
        else if(msg._bytesRemaining>0)
            return(TASTY_PENDING);
        else
            throw; //shouldn't happen, something wrong with internal counting
    }
    else if(bytesSent==0) //Connection shutdown
        return(TASTY_NOCONNECTION);
    else
        return(TASTY_ERROR); //Check errno, something else went wrong
}
