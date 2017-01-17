#ifndef _NETWORK_COMMON_H_
#define _NETWORK_COMMON_H_

#include "network_packet.h"

class ConnectionRequestPacket : public Packet
{
public:
	ConnectionRequestPacket();
};

class ConnectionDeniedPacket : public Packet
{
public:
	ConnectionDeniedPacket();

};

class ConnectionPacket : public Packet
{
public:
	ConnectionPacket() {}

};
class ConnectionAcceptPacket : public Packet
{
public:
	ConnectionAcceptPacket();

};

class ConnectionDisconnectPacket : public Packet
{
public:
	ConnectionDisconnectPacket();

};

enum ClientServerPacketTypes
{
	CLIENT_SERVER_PACKET_REQUEST = 0,	// Client request to server
	CLIENT_SERVER_PACKET_DENIED,		// Server response to client
	CLIENT_SERVER_PACKET_ACCEPTED,		// Server response to client
	CLIENT_SERVER_PACKET_CONNECTION,	// Normal message communication
	CLIENT_SERVER_PACKET_DISCONNECT,	// Disconnect current connection
	CLIENT_SERVER_MAX_PACKETS
};

PACKET_FACTORY_START(SnapshotPacketFactory, PacketFactory, CLIENT_SERVER_MAX_PACKETS);
	PACKET_FACTORY_TYPE(CLIENT_SERVER_PACKET_REQUEST, ConnectionRequestPacket);
	PACKET_FACTORY_TYPE(CLIENT_SERVER_PACKET_DENIED, ConnectionDeniedPacket);
	PACKET_FACTORY_TYPE(CLIENT_SERVER_PACKET_ACCEPTED, ConnectionAcceptPacket);
	PACKET_FACTORY_TYPE(CLIENT_SERVER_PACKET_CONNECTION, ConnectionPacket);
	PACKET_FACTORY_TYPE(CLIENT_SERVER_PACKET_DISCONNECT, ConnectionDisconnectPacket);
PACKET_FACTORY_END();


#endif