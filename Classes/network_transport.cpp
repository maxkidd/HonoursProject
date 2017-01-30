#include "network_transport.h"



SocketTransport::SocketTransport(PacketFactory * packetFactory) : io_service_(),
socket_(io_service_, udp::endpoint(udp::v4(), 0)), _packetFactory(packetFactory)
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
	asio::error_code error;
	int flags = 0;

	socket_.send_to(asio::buffer(data, size), endpoint, flags, error);
	return true;
}

Packet * BaseTransport::CreatePacket()
{
	return nullptr;
}

Packet * BaseTransport::ReceivePacket()
{
	if (!receive_queue_.empty())
	{
		Packet* packet = receive_queue_.front();
		receive_queue_.pop();

		return packet;
	}
	return nullptr;
}

void BaseTransport::SendPacket(const udp::endpoint & endpoint, Packet* data)
{
	if (send_queue_.size() <= MAX_SEND_QUEUE)
	{
		send_queue_.push(data);
	}

	InternalSendPacket(endpoint, data, 1024);
}

void BaseTransport::WritePackets()
{
	while (!send_queue_.empty())
	{
		Packet* packet = send_queue_.front();
		send_queue_.pop();
		
		// Address

		//WritePacket(packet, buffer, size)

	}
}

void BaseTransport::ReadPackets()
{
}
