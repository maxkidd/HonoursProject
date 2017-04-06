#ifndef _NETWORK_COMMON_H_
#define _NETWORK_COMMON_H_

#include "network_packet.h"
#include "network_message.h"
#include "network_channel.h"

#include <algorithm>

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


PACKET_FACTORY_START(UnreliablePacketFactory, PacketFactory, CLIENT_SERVER_MAX_PACKETS);
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
	float x, y = 0.0f;
	uint32_t rot = 0;

	template<typename Stream> bool Serialize(Stream& stream)
	{
		stream.SerializeInteger(id, 0, 255);

		SerializeFloat(stream, x);
		SerializeFloat(stream, y);
		stream.SerializeInteger(rot, 0, 360);

		return true;
	}
	VIRTUAL_SERIALIZE_FUNCTIONS();
};

class SnapshotBoxMove : public NMessage
{
public:
	uint32_t id = 0;
	float x, y = 0.0f;
	uint32_t rot = 0;

	template<typename Stream> bool Serialize(Stream& stream)
	{
		stream.SerializeInteger(id, 0, 255);

		SerializeFloat(stream, x);
		SerializeFloat(stream, y);
		stream.SerializeInteger(rot, 0, 360);
		
		return true;
	}
	VIRTUAL_SERIALIZE_FUNCTIONS();
};


class StateSyncBoxCreate : public NMessage
{
public:
	uint32_t id = 0;
	float x, y = 0.0f;
	uint32_t rot = 0;

	template<typename Stream> bool Serialize(Stream& stream)
	{
		stream.SerializeInteger(id, 0, 255);

		SerializeFloat(stream, x);
		SerializeFloat(stream, y);
		stream.SerializeInteger(rot, 0, 360);

		return true;
	}
	VIRTUAL_SERIALIZE_FUNCTIONS();
};
class StateSyncBoxMove : public NMessage
{
public:
	uint32_t id = 0;
	float x, y = 0.0f;
	uint32_t rot = 0;

	float velocityX, velocityY = 0.0f;
	float rotVel = 0.0f;

	template<typename Stream> bool Serialize(Stream& stream)
	{
		stream.SerializeInteger(id);

		SerializeFloat(stream, x);
		SerializeFloat(stream, y);
		stream.SerializeInteger(rot, 0, 360);
		
		// Velocity data
		if (Stream::IsWriting)
		{
			// Clamp
			velocityX = velocityX + 50.0f;
			velocityX = std::max(std::min(velocityX, 100.0f), 0.0f) * 100.0f;

			velocityY = velocityY + 50.0f;
			velocityY = std::max(std::min(velocityY, 100.0f), 0.0f) * 100.0f;
		}

		uint32_t i_velocityX = (uint32_t)floor(velocityX);
		uint32_t i_velocityY = (uint32_t)floor(velocityY);

		stream.SerializeInteger(i_velocityX, 0, 10000);
		stream.SerializeInteger(i_velocityY, 0, 10000);

		if (Stream::IsReading)
		{
			velocityX = (float(i_velocityX) / 100.0f) - 50.0f;
			velocityY = (float(i_velocityY) / 100.0f) - 50.0f;
		}

		SerializeFloat(stream, rotVel);

		return true;
	}
	VIRTUAL_SERIALIZE_FUNCTIONS();
};

class UserCMD : public NMessage
{
public:
	uint32_t mDown = false;
	float mX, mY = 0;

	template<typename Stream> bool Serialize(Stream& stream)
	{
		stream.SerializeBits(mDown, 1);
		if (mDown)
		{
			SerializeFloat(stream, mX);
			SerializeFloat(stream, mY);
		}

		return true;
	}
	VIRTUAL_SERIALIZE_FUNCTIONS();
};

enum SnapshotMessageTypes
{
	SNAPSHOT_MESSAGE_ERROR = -1,
	SNAPSHOT_MESSAGE_MOVE_BOX = 0,		// Box message
	SNAPSHOT_MESSAGE_CREATE_BOX,		// Box message
	SNAPSHOT_MESSAGE_USER_CMD,
	SNAPSHOT_MESSAGE_MAX
};
enum StateSyncMessageTypes
{
	STATESYNC_MESSAGE_ERROR = -1,
	STATESYNC_MESSAGE_UPDATE_BOX = 0,		// Box message
	STATESYNC_MESSAGE_CREATE_BOX,		// Box message
	STATESYNC_MESSAGE_USER_CMD,
	STATESYNC_MESSAGE_MAX
};

MESSAGE_FACTORY_START(SnapshotMessageFactory, MessageFactory, SNAPSHOT_MESSAGE_MAX);
	MESSAGE_FACTORY_TYPE(SNAPSHOT_MESSAGE_MOVE_BOX, SnapshotBoxMove);
	MESSAGE_FACTORY_TYPE(SNAPSHOT_MESSAGE_CREATE_BOX, SnapshotBoxCreate);
	MESSAGE_FACTORY_TYPE(SNAPSHOT_MESSAGE_USER_CMD, UserCMD);
MESSAGE_FACTORY_END();
MESSAGE_FACTORY_START(StateSyncMessageFactory, MessageFactory, STATESYNC_MESSAGE_MAX);
	MESSAGE_FACTORY_TYPE(STATESYNC_MESSAGE_UPDATE_BOX, StateSyncBoxMove);
	MESSAGE_FACTORY_TYPE(STATESYNC_MESSAGE_CREATE_BOX, StateSyncBoxCreate);
	MESSAGE_FACTORY_TYPE(STATESYNC_MESSAGE_USER_CMD, UserCMD);
MESSAGE_FACTORY_END();

#endif