#ifndef _NETWORKCHANNEL_H_
#define _NETWORKCHANNEL_H_

#include <queue>

#include "network_message.h"

class ChannelPacket
{
	friend class Channel;
private:

	int numMessages;
	std::vector<Message*> messages;

	// Serialize functions
	template <typename Stream> bool Serialize(Stream& stream, MessageFactory& mf, int channels); 
	
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

	ChannelPacket* GeneratePacketData(int freeBits);
	int GetPacketData(ChannelPacket& data, int bitsFree);
	void ProcessPacketData(const ChannelPacket& data);

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

