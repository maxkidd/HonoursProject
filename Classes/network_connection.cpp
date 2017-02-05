#include "network_connection.h"

#include "network_common.h"


Connection::Connection(udp::endpoint endpoint) : _endpoint(endpoint)
{
}

Connection::~Connection()
{
}

ConnectionPacket * Connection::GeneratePacket()
{
	ConnectionPacket* packet = new ConnectionPacket();

	// Add messages to connection packet

	return packet;
}

udp::endpoint Connection::Endpoint()
{
	return _endpoint;
}
