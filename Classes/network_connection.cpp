#include "network_connection.h"

#include "network_common.h"

Connection::Connection()
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
