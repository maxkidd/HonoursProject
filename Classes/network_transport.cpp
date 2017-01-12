#include "network_transport.h"


SocketTransport::SocketTransport() : io_service_(),
	socket_(io_service_, udp::endpoint(udp::v4(), 0))
{
	
}

bool SocketTransport::InternalReceivePacket(udp::endpoint & endpoint, void * data, int bytes)
{
	//int result = socket_.receive(asio::buffer(data, bytes));
	size_t result;// = socket_.receive_from(asio::buffer(data, bytes), endpoint);

	// Error checking

	// return bytes recvd
	return result;
}

bool SocketTransport::InternalSendPacket(const udp::endpoint & endpoint, const void * data, int size)
{
	socket_.send_to(asio::buffer(data, size), endpoint);
	return true;
}

Packet * BaseTransport::CreatePacket()
{
	return nullptr;
}

Packet * BaseTransport::ReceivePacket()
{
	return nullptr;
}

void BaseTransport::SendPacket(const udp::endpoint & endpoint, Packet * packet)
{
	InternalSendPacket(endpoint, packet->data, 1024);
}

void BaseTransport::WritePackets()
{
}

void BaseTransport::ReadPackets()
{
}
