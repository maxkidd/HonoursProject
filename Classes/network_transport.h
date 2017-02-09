#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include <queue>
#include "asio\asio.hpp"

#include "network_packet.h"

#include "NetworkDebugDataSource.h"

using asio::ip::udp;

#define MAX_SEND_QUEUE (100)
#define MAX_RECEIVE_QUEUE (100)

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
	BaseTransport(PacketFactory* packetFactory) : _packetFactory(packetFactory), max_packet_size_(100) {}
	virtual ~BaseTransport() {}

	Packet* CreatePacket();
	Packet* ReceivePacket(udp::endpoint &endpoint);

	void SendPacket(const udp::endpoint & endpoint, Packet * data);

	void WritePackets();
	void ReadPackets();

	void SetDebugService(NetworkDebugDataSource* dataSource) { _debugData = dataSource; }
protected:
	// Internal receive functions to recieve packets from destination
	virtual int InternalReceivePacket(udp::endpoint & endpoint, void * data, int bytes) = 0;
	// Internal send function to transport packets to a destination
	virtual bool InternalSendPacket(const udp::endpoint & endpoint, const void * data, int size) = 0;

private:
	std::queue<PacketInfo> send_queue_;
	std::queue<PacketInfo> receive_queue_;

	PacketFactory* _packetFactory;

	NetworkDebugDataSource* _debugData;

	int max_packet_size_;
};

class SocketTransport : public BaseTransport
{
public:
	//SocketTransport(PacketFactory* packetFactory);
	SocketTransport(PacketFactory* packetFactory, unsigned short port = 0);
	asio::io_service* GetIOService() { return &io_service_; }
protected:
	virtual int InternalReceivePacket(udp::endpoint & endpoint, void * data, int bytes);
	virtual bool InternalSendPacket(const udp::endpoint & endpoint, const void * data, int size);

private:
	asio::io_service io_service_;
	udp::socket socket_;
};

#endif
