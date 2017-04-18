#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include <queue>

#include "asio\asio.hpp"

#include "network_packet.h"
#include "network_message.h"


using asio::ip::udp;


#define MAX_SEND_QUEUE (100)	// Maximum messages in the send queue
#define MAX_RECEIVE_QUEUE (100)	// Maximum messages in the recv queue

class NetworkLog;

/**
	Added info regarding packets sent/recv
*/
struct PacketInfo
{
	PacketInfo()
	{
		packet = nullptr;
	}
	udp::endpoint endpoint;
	Packet* packet;
};
/**
	Base class for sending and receiving packets

	Different transport implementations can be setup
*/
class BaseTransport
{
public:
	BaseTransport(PacketFactory* packetFactory, MessageFactory* messageFactory)
		: _packetFactory(packetFactory), _messageFactory(messageFactory), max_packet_size_(1900)
	{
		_context._packetFactory = packetFactory;
		_context._messageFactory = messageFactory;
	}
	virtual ~BaseTransport() {}
	asio::io_service* GetIOService() { return &_IOService; }


	// Receive the next packet and corresponding endpoint that has been read
	Packet* ReceivePacket(udp::endpoint &endpoint);
	// Send packet to the send queue waiting to be written
	void SendPacket(const udp::endpoint & endpoint, Packet * data);
	// Read&Write packets using the inherited internal functions
	void WritePackets();
	void ReadPackets();
protected:
	// Internal receive functions to recieve packets from destination
	virtual int InternalReceivePacket(udp::endpoint & endpoint, void * data, int bytes) = 0;
	// Internal send function to transport packets to a destination
	virtual bool InternalSendPacket(const udp::endpoint & endpoint, const void * data, int size) = 0;

private:
	std::queue<PacketInfo> _sendQueue;
	std::queue<PacketInfo> _receiveQueue;

	PacketFactory* _packetFactory;
	MessageFactory* _messageFactory;

	StreamContext _context;
	int max_packet_size_;
protected:
	asio::io_service _IOService;
};

/**
	Transport using ASIO to send/recv packets over UDP
*/
class SocketTransport : public BaseTransport
{
public:
	SocketTransport(PacketFactory * packetFactory, MessageFactory* messageFactory, unsigned short port = 0);
	virtual ~SocketTransport();

	/// Returns local port used
	unsigned short GetPort();

protected:
	/// Override receive packet -> ASIO
	virtual int InternalReceivePacket(udp::endpoint & endpoint, void * data, int bytes);
	/// Override send packet -> ASIO
	virtual bool InternalSendPacket(const udp::endpoint & endpoint, const void * data, int size);

private:
	udp::socket _socket;
};

#endif
