#ifndef _NETWORKCHANNEL_H_
#define _NETWORKCHANNEL_H_

#include <queue>

#include "network_message.h"

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

class ChannelListener
{
public:
	virtual ~ChannelListener() {}
	virtual void OnReceive(class Channel*) {}
};

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

