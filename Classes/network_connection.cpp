#include "network_connection.h"

#include "network_common.h"



Connection::Connection(udp::endpoint endpoint, PacketFactory & pf, MessageFactory * mf)
	: _endpoint(endpoint), _packetFactory(&pf), _messageFactory(mf)
{
	_channel = new Channel(mf);
	_lastProcessedPacket = std::chrono::high_resolution_clock::now();
}

Connection::~Connection()
{
	delete _channel;
}

bool Connection::HasTimedOut()
{
	std::chrono::duration<float> elapsed = std::chrono::high_resolution_clock::now() - _lastProcessedPacket;
	if (elapsed.count() > TIMEOUT)
		return true;
	return false;
}

ConnectionPacket * Connection::GeneratePacket()
{
	ConnectionPacket* packet = (ConnectionPacket*)_packetFactory->Create(CLIENT_SERVER_PACKET_CONNECTION);

	// Add messages to connection packet from channel layer
	ChannelPacket* channel_packet = new ChannelPacket();
	_channel->GetPacketData(*channel_packet, 0);

	memcpy(&packet->_channelEntry, &channel_packet, sizeof(ChannelPacket*));
	memcpy(&packet->_messageFactory, &_messageFactory, sizeof(MessageFactory*));

	return packet;
}

bool Connection::ProcessPacket(ConnectionPacket * packet)
{
	// Chrono timepoint
	_lastProcessedPacket = std::chrono::high_resolution_clock::now();

	_channel->ProcessPacketData(*(packet->_channelEntry));

	return true;
}

void Connection::SendMsg(NMessage * message)
{
	_channel->SendMsg(message);
}

NMessage * Connection::ReceiveMsg()
{
	return _channel->ReceiveMsg();
}

udp::endpoint Connection::Endpoint()
{
	return _endpoint;
}
