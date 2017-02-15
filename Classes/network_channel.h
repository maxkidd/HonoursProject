#ifndef _NETWORKCHANNEL_H_
#define _NETWORKCHANNEL_H_

#include <queue>

#include "network_message.h"

class ChannelData
{
	// Serialize functions
};

class ChannelListener
{
public:
	virtual ~ChannelListener() {}
	virtual void OnReceive(class Channel*) {}
};

class Channel
{
public:
	Channel(MessageFactory* mf, int id);
	~Channel() {}
	
	void SendMessage(Message* message);
	Message* ReceiveMessage();

	//ChannelData* GenerateChannelData();
	//void ProcessPacketData(const ChannelData& data);

	void SetListener(ChannelListener *  listener);
protected:
private:
	int _id;

	ChannelListener* _listener;

	std::queue<Message*> _recvQueue;
	std::queue<Message*> _sendQueue;

	MessageFactory* _mf;
};

#endif