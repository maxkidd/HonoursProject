#include "network_transport.h"

#include <cocos2d.h>

SocketTransport::SocketTransport(PacketFactory * packetFactory, MessageFactory* messageFactory, unsigned short port) 
	: BaseTransport(packetFactory, messageFactory), socket_(_io_service, udp::endpoint(udp::v4(), port))
{
	socket_.non_blocking(true);
}

unsigned short SocketTransport::GetPort()
{
	return socket_.local_endpoint().port();
}


int SocketTransport::InternalReceivePacket(udp::endpoint & endpoint, void * data, int bytes)
{
	//int result = socket_.receive(asio::buffer(data, bytes));
	asio::error_code ec;

	size_t result = socket_.receive_from(asio::buffer(data, bytes), endpoint, 0, ec);
	if(!result)
		CCLOG("InternalReceivePacket : %s", ec.message());


	// return bytes recvd
	return result;
}

bool SocketTransport::InternalSendPacket(const udp::endpoint & endpoint, const void * data, int size)
{
	asio::error_code ec;
	int flags = 0;

	int bytesSent = socket_.send_to(asio::buffer(data, size), endpoint, flags, ec);

	if(!bytesSent)
		CCLOG("InternalSendPacket : %s", ec.message());

	return true;
}

Packet * BaseTransport::CreatePacket()
{
	return nullptr;
}

Packet * BaseTransport::ReceivePacket(udp::endpoint & endpoint)
{
	if (!receive_queue_.empty())
	{
		PacketInfo packetInfo = receive_queue_.front();
		receive_queue_.pop();

		endpoint = packetInfo.endpoint;

		return packetInfo.packet;
	}
	return nullptr;
}

void BaseTransport::SendPacket(const udp::endpoint & endpoint, Packet* data)
{
	if (send_queue_.size() <= MAX_SEND_QUEUE)
	{
		PacketInfo packetInfo;
		packetInfo.packet = data;
		packetInfo.endpoint = endpoint;
		send_queue_.push(packetInfo);
	}
}

void BaseTransport::WritePackets()
{
	// Iterate through all send queue
	while (!send_queue_.empty())
	{
		// Pop next packet to send
		PacketInfo packetInfo = send_queue_.front();
		send_queue_.pop();
		
		// Write packet to temporary buffer
		uint8_t* buffer = new uint8_t[max_packet_size_];
		memset(buffer, 0, sizeof(uint8_t)*max_packet_size_);

		int bytesUsed = WritePacket(&_context, packetInfo.packet, buffer, max_packet_size_);

		// Send packet to address
		InternalSendPacket(packetInfo.endpoint, buffer, bytesUsed);
		
		
		_debugData.createEntry(to_string(packetInfo.packet->GetType()) + " Sent " + std::to_string(bytesUsed) + "bytes to "
			+ packetInfo.endpoint.address().to_string(), NET_LOG);
	}
}

void BaseTransport::ReadPackets()
{
	while (true)
	{
		uint8_t* buffer = new uint8_t[max_packet_size_];
		memset(buffer, 0, sizeof(uint8_t)*max_packet_size_);

		asio::ip::basic_endpoint<udp> endpoint;

		int bytesReceived = InternalReceivePacket(endpoint, buffer, max_packet_size_);
		
		// Finish if no bytes received
		if (!bytesReceived)
			break;

		if (receive_queue_.size() >= MAX_RECEIVE_QUEUE)
			break;

		// All is good
		PacketInfo packetInfo;
		packetInfo.endpoint = endpoint;
		packetInfo.packet = ReadPacket(&_context, _packetFactory, buffer, bytesReceived);
		
		receive_queue_.push(packetInfo);


		_debugData.createEntry(to_string(packetInfo.packet->GetType()) + " Received " + std::to_string(bytesReceived) + "bytes from "
			+ packetInfo.endpoint.address().to_string(), NET_LOG);
	}
}
