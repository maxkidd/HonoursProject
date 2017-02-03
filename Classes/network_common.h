#ifndef _NETWORK_COMMON_H_
#define _NETWORK_COMMON_H_

#include "network_packet.h"

class ConnectionRequestPacket : public Packet
{
public:
	ConnectionRequestPacket() {}


	template<typename Stream> bool Serialize(Stream& stream)
	{

		return true;
	}
	VIRTUAL_SERIALIZE_FUNCTIONS();
};

class ConnectionDeniedPacket : public Packet
{
public:
	ConnectionDeniedPacket() {}

	template<typename Stream> bool Serialize(Stream& stream)
	{

		return true;
	}
	VIRTUAL_SERIALIZE_FUNCTIONS();
};

class ConnectionPacket : public Packet
{
public:
	ConnectionPacket() {}

	template<typename Stream> bool Serialize(Stream& stream)
	{
		return true;
	}
	VIRTUAL_SERIALIZE_FUNCTIONS();
};
class ConnectionAcceptPacket : public Packet
{
public:
	ConnectionAcceptPacket() {}

	template<typename Stream> bool Serialize(Stream& stream)
	{

		return true;
	}
	VIRTUAL_SERIALIZE_FUNCTIONS();
};

class ConnectionDisconnectPacket : public Packet
{
public:
	ConnectionDisconnectPacket() {}

	template<typename Stream> bool Serialize(Stream& stream)
	{

		return true;
	}
	VIRTUAL_SERIALIZE_FUNCTIONS();
};

enum ClientServerPacketTypes
{
	CLIENT_SERVER_PACKET_ERROR = -1,
	CLIENT_SERVER_PACKET_NULL = 0,
	CLIENT_SERVER_PACKET_REQUEST,		// Client request to server
	CLIENT_SERVER_PACKET_DENIED,		// Server response to client
	CLIENT_SERVER_PACKET_ACCEPTED,		// Server response to client
	CLIENT_SERVER_PACKET_CONNECTION,	// Normal message communication
	CLIENT_SERVER_PACKET_DISCONNECT,	// Disconnect current connection
	CLIENT_SERVER_MAX_PACKETS
};

PACKET_FACTORY_START(SnapshotPacketFactory, PacketFactory, CLIENT_SERVER_MAX_PACKETS);
	PACKET_FACTORY_TYPE(CLIENT_SERVER_PACKET_NULL, ConnectionRequestPacket);
	PACKET_FACTORY_TYPE(CLIENT_SERVER_PACKET_REQUEST, ConnectionRequestPacket);
	PACKET_FACTORY_TYPE(CLIENT_SERVER_PACKET_DENIED, ConnectionDeniedPacket);
	PACKET_FACTORY_TYPE(CLIENT_SERVER_PACKET_ACCEPTED, ConnectionAcceptPacket);
	PACKET_FACTORY_TYPE(CLIENT_SERVER_PACKET_CONNECTION, ConnectionPacket);
	PACKET_FACTORY_TYPE(CLIENT_SERVER_PACKET_DISCONNECT, ConnectionDisconnectPacket);
PACKET_FACTORY_END();


#endif