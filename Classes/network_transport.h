#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include <queue>
#include "asio\asio.hpp"

using asio::ip::udp;

class Packet;

class BaseTransport
{
public:
	virtual ~BaseTransport() {}

	Packet* CreatePacket();
	Packet* ReceivePacket();

	void SendPacket(const udp::endpoint & endpoint, Packet * packet);

	void WritePackets();
	void ReadPackets();

protected:
	// Internal receive functions to recieve packets from destination
	virtual bool InternalReceivePacket(const udp::endpoint & endpoint, const void * data, int bytes) = 0;
	// Internal send function to transport packets to a destination
	virtual bool InternalSendPacket(udp::endpoint & endpoint, void * data, int size) = 0;

private:
	std::queue<Packet> send_queue_;
	std::queue<Packet> receive_queue_;

};

class SocketTransport : public BaseTransport
{
public:

protected:
	virtual bool InternalReceivePacket(udp::endpoint & endpoint, void * data, int bytes);
	virtual bool InternalSendPacket(const udp::endpoint & endpoint, const void * data, int size);

private:
	asio::io_service io_service_;
	udp::socket socket_;


};

#endif
