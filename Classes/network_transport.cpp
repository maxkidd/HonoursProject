#include "network_transport.h"

bool SocketTransport::InternalReceivePacket(udp::endpoint & endpoint, void * data, int bytes)
{
	int result = socket_.receive(asio::buffer(data, bytes));

	// Error checking

	// return bytes recvd
	return result;
}

bool SocketTransport::InternalSendPacket(const udp::endpoint & endpoint, const void * data, int size)
{
	socket_.send_to(asio::buffer(data, size), endpoint);
	return true;
}
