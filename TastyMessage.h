/*****************************************************************************/
// Filename: TastyMessage.h
/*****************************************************************************/
// Description: Defines the type of messages TastyStreams can send/receive.
// TastyMessage essentially is just a character buffer that keeps track
// of number of bytes received or sent
/*****************************************************************************/

#ifndef TASTY_MESSAGE_H_DEFINED
#define TASTY_MESSAGE_H_DEFINED

#include <cstring>

struct TastyMessage
{
    friend class CTastyStream;

        //Use this constructor Receive
        TastyMessage(unsigned int msgSize) : _bytesRemaining(msgSize), _messageSize(msgSize)
        {
            _buffer = new char[msgSize];
        }

        //Use this constructor for Send...can also be used to receive I guess, but msgSize will be message.size()
        TastyMessage(string message)
        {
            _buffer = new char[message.size()];
            memcpy(_buffer, message.c_str(), message.size());
            _bytesRemaining = message.size();
            _messageSize = message.size();
        }

        //Destructor which frees up underlying buffer
        ~TastyMessage()
        {
            if(_buffer)
                delete[](_buffer);
        }

        unsigned int BytesRemaining(){return(_bytesRemaining);}
        char*        Buffer(){return(_buffer);}
        unsigned int MessageSize(){return(_messageSize);}

    private:
        unsigned int _bytesRemaining;
        char*        _buffer;
        unsigned int _messageSize;
};

#endif
