#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include <queue>
#include "asio\asio.hpp"

#include "network_packet.h"
#include "network_message.h"

//#include "NetworkDebugDataSource.h"

#include "ImGUI\CCIMGUI.h"

using asio::ip::udp;

#define MAX_SEND_QUEUE (100)
#define MAX_RECEIVE_QUEUE (100)

class NetworkLog;

struct PacketInfo
{
	PacketInfo()
	{
		packet = nullptr;
	}
	udp::endpoint endpoint;
	Packet* packet;
};
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

	Packet* CreatePacket();
	Packet* ReceivePacket(udp::endpoint &endpoint);

	void SendPacket(const udp::endpoint & endpoint, Packet * data);

	void WritePackets();
	void ReadPackets();

	asio::io_service* GetIOService() { return &_IOService; }
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

	NetworkLog* log;

};

class SocketTransport : public BaseTransport
{
public:
	//SocketTransport(PacketFactory* packetFactory);
	SocketTransport(PacketFactory * packetFactory, MessageFactory* messageFactory, unsigned short port = 0);
	virtual ~SocketTransport();

	unsigned short GetPort();
protected:
	virtual int InternalReceivePacket(udp::endpoint & endpoint, void * data, int bytes);
	virtual bool InternalSendPacket(const udp::endpoint & endpoint, const void * data, int size);

private:
	udp::socket _socket;

	// ImGUI
	bool opened = true;
};

#endif
