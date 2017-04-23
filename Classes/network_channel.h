#ifndef _NETWORKCHANNEL_H_
#define _NETWORKCHANNEL_H_

#include <queue>

#include "network_message.h"

/**
	Channel packet packs messages sent from the simulation layer
	- Created from the connection packet
*/
class ChannelPacket
{
public:
	// Serialize functions
	template <typename Stream> bool Serialize(Stream& stream, MessageFactory* mf, int channels);

	bool SerializeInternal(InStream& stream, MessageFactory* mf, int channels);
	bool SerializeInternal(OStream& stream, MessageFactory* mf, int channels);

private:
	friend class Channel;

	std::vector<NMessage*> messages;
	uint32_t numMessages = 0;
};

/**
	Channel listener
	- For callbacks about channel messages received
	- Not fully implemented yet
*/
class ChannelListener
{
public:
	virtual ~ChannelListener() {}
	virtual void OnReceive(class Channel*) {}
};

/**
	Channel
	- Stores messages in a send and receive queue
	- Receives packet data
	- Generates channel packet for connection packets with messages in the send queue
*/
class Channel
{
public:
	Channel(MessageFactory* mf);
	~Channel() {}
	
	void SendMsg(NMessage* message);
	NMessage* ReceiveMsg();

	int GetPacketData(ChannelPacket& data, int bitsFree);
	void ProcessPacketData(const ChannelPacket& data);

	void SetListener(ChannelListener *  listener);
protected:
private:
	ChannelListener* _listener;

	std::queue<NMessage*> _recvQueue;
	std::queue<NMessage*> _sendQueue;

	MessageFactory* _mf;
};

#endif

