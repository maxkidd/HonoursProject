#ifndef _NETWORK_CONNECTION_H_
#define _NETWORK_CONNECTION_H_

#include "network_packet.h"
#include "network_message.h"
#include "network_channel.h"

#include "asio\asio.hpp"

class ConnectionPacket;

using namespace asio::ip;

class Connection
{
public:
	Connection(udp::endpoint endpoint, PacketFactory& pf, MessageFactory* mf);
	~Connection();

	ConnectionPacket* GeneratePacket();
	bool ProcessPacket(ConnectionPacket* packet);

	void SendMsg(NMessage* message);
	NMessage* ReceiveMsg();

	udp::endpoint Endpoint();
protected:
private:
	udp::endpoint _endpoint;

	Channel* _channel;

	PacketFactory* _packetFactory;
	MessageFactory* _messageFactory;
};

#endif