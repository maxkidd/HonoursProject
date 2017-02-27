#include "network_connection.h"

#include "network_common.h"



Connection::Connection(udp::endpoint endpoint, PacketFactory & pf, MessageFactory & mf)
	: _endpoint(endpoint), _packetFactory(&pf), _messageFactory(&mf)
{
	_channel = new Channel(&mf);
}

Connection::~Connection()
{
	delete _channel;
}

ConnectionPacket * Connection::GeneratePacket()
{
	ConnectionPacket* packet = new ConnectionPacket();

	// Send message factory
	packet->_messageFactory = _messageFactory;
	// Add messages to connection packet
	memcpy(&packet->_channelEntry, &_channel, sizeof(ChannelPacket));


	return packet;
}

bool Connection::ProcessPacket(ConnectionPacket * packet)
{
	_channel->ProcessPacketData(*packet->_channelEntry);

	return true;
}

void Connection::SendMessage(NMessage * message)
{
	_channel->SendMsg(message);
}

NMessage * Connection::ReceiveMessage()
{
	return _channel->ReceiveMsg();
}

udp::endpoint Connection::Endpoint()
{
	return _endpoint;
}
