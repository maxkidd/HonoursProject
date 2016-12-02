#ifndef _NETWORKCHANNEL_H_
#define _NETWORKCHANNEL_H_

#include <queue>

#include "network_message.h"

class ChannelListener
{
public:
	virtual ~ChannelListener() {}
	virtual void OnReceive(class Channel*) {}
};

class Channel
{
public:
	Channel();
	~Channel();
	
	void SendMessage(Message* message);
	void SetListener(ChannelListener *  listener);
protected:
private:
	int id;

	ChannelListener* _listener;

	std::queue<Message*> _recvQueue;
	std::queue<Message*> _sendQueue;
};

#endif