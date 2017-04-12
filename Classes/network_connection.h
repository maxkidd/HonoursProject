#ifndef _NETWORK_CONNECTION_H_
#define _NETWORK_CONNECTION_H_

#include "network_packet.h"
#include "network_message.h"
#include "network_channel.h"

#include "asio\asio.hpp"

#include <chrono>

class ConnectionPacket;

using namespace asio::ip;

#define TIMEOUT 2.0f

class Connection
{
public:
	Connection(udp::endpoint endpoint, PacketFactory& pf, MessageFactory* mf);
	~Connection();

	bool HasTimedOut();

	ConnectionPacket* GeneratePacket();
	bool ProcessPacket(ConnectionPacket* packet);

	void ProcessAcks(uint32_t ack, uint32_t prevAcks);

	void SendMsg(NMessage* message);
	NMessage* ReceiveMsg();

	udp::endpoint Endpoint();
protected:
private:
	udp::endpoint _endpoint;

	Channel* _channel;

	PacketFactory* _packetFactory;
	MessageFactory* _messageFactory;

	std::chrono::high_resolution_clock::time_point _lastProcessedPacket;


	const int sequenceBufferSize = 64;

	uint32_t sentSequence = 1;
	bool* sentAckEntries;

	uint32_t recvSequence = 1;
	bool* recvAckEntries;


};

#endif