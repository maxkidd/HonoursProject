#include "network_channel.h"

Channel::Channel(MessageFactory * mf, int id) : _mf(mf), _id(id)
{
}

void Channel::SendMessage(Message * message)
{
	_sendQueue.push(message);
}

Message * Channel::ReceiveMessage()
{
	Message* returnMsg = _recvQueue.front();
	_recvQueue.pop();
	return returnMsg;
}
