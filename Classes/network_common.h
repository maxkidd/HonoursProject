#ifndef _NETWORK_COMMON_H_
#define _NETWORK_COMMON_H_

#include "network_packet.h"
#include "network_message.h"
#include "network_channel.h"

class ChannelPacket;

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
	ChannelPacket* _channelEntry = nullptr; // Channel packet that holds messages
	MessageFactory* _messageFactory = nullptr;

	ConnectionPacket() { }

	template<typename Stream> bool Serialize(Stream& stream)
	{

		if (Stream::IsReading)
		{
			StreamContext* streamContext = stream.GetContext();
			_channelEntry = new ChannelPacket();
			_messageFactory = streamContext->_messageFactory;
		}

		assert(_messageFactory);
		assert(_channelEntry);

		if (!_channelEntry->Serialize(stream, _messageFactory, 1))
		{
			// Could not serilize
			return false;
		}

		return true;
	}
	VIRTUAL_SERIALIZE_FUNCTIONS();


	void SetMessageFactory(MessageFactory& mf) { _messageFactory = &mf; }

};
class ConnectionAcceptPacket : public Packet
{
public:
	uint32_t test = 0;

	ConnectionAcceptPacket() {}

	template<typename Stream> bool Serialize(Stream& stream)
	{
		stream.SerializeInteger(test);

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


class SnapshotBoxCreate : public NMessage
{
public:
	uint32_t id = 0;
	float x, y = 0;

	template<typename Stream> bool Serialize(Stream& stream)
	{
		stream.SerializeInteger(id, 0, 255);

		SerializeFloat(stream, x);
		//SerializeFloat(stream, y);

		return true;
	}
	VIRTUAL_SERIALIZE_FUNCTIONS();
};
class SnapshotBoxMove : public NMessage
{
public:
	uint32_t id = 0;
	float x, y = 0;

	template<typename Stream> bool Serialize(Stream& stream)
	{
		stream.SerializeInteger(id);

		//SerializeFloat(stream, x);
		SerializeFloat(stream, y);
		
		return true;
	}
	VIRTUAL_SERIALIZE_FUNCTIONS();
};

enum SnapshotMessageTypes
{
	SNAPSHOT_MESSAGE_ERROR = -1,
	SNAPSHOT_MESSAGE_MOVE_BOX = 0,		// Box message
	SNAPSHOT_MESSAGE_CREATE_BOX,		// Box message
	SNAPSHOT_MESSAGE_MAX
};

MESSAGE_FACTORY_START(SnapshotMessageFactory, MessageFactory, SNAPSHOT_MESSAGE_MAX);
	MESSAGE_FACTORY_TYPE(SNAPSHOT_MESSAGE_MOVE_BOX, SnapshotBoxMove);
	MESSAGE_FACTORY_TYPE(SNAPSHOT_MESSAGE_CREATE_BOX, SnapshotBoxCreate);
MESSAGE_FACTORY_END();

#endif