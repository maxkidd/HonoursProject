#include "network_transport.h"

#include <cocos2d.h>

#include "ImGui\CCIMGUI.h"

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

SocketTransport::SocketTransport(PacketFactory * packetFactory, MessageFactory* messageFactory, unsigned short port) 
	: BaseTransport(packetFactory, messageFactory), _socket(_IOService, udp::endpoint(udp::v4(), port))
{
	_socket.non_blocking(true);


	CCIMGUI::getInstance()->addImGUI([=]() {

		/*static float last_time = -1.0f;
		float time = ImGui::GetTime();
		if (time - last_time >= 0.3f)
		{
			const char* random_words[] = { "system", "info", "warning", "error", "fatal", "notice", "log" };
			log.AddLog("[%s] Hello, time is %.1f, rand() %d\n", random_words[rand() % IM_ARRAYSIZE(random_words)], time, (int)rand());
			last_time = time;
		}*/

		log.Draw("Network Log", &opened);

	}, "Logging");

}

SocketTransport::~SocketTransport()
{
	CCIMGUI::getInstance()->removeImGUI("Logging");
}

unsigned short SocketTransport::GetPort()
{
	return _socket.local_endpoint().port();
}


int SocketTransport::InternalReceivePacket(udp::endpoint & endpoint, void * data, int bytes)
{
	asio::error_code ec;

	size_t result = _socket.receive_from(asio::buffer(data, bytes), endpoint, 0, ec);
	if(!result)
		CCLOG("InternalReceivePacket : %s", ec.message());


	// return bytes recvd
	return result;
}

bool SocketTransport::InternalSendPacket(const udp::endpoint & endpoint, const void * data, int size)
{
	asio::error_code ec;
	int flags = 0;

	int bytesSent = _socket.send_to(asio::buffer(data, size), endpoint, flags, ec);

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
	if (!_receiveQueue.empty())
	{
		PacketInfo packetInfo = _receiveQueue.front();
		_receiveQueue.pop();

		endpoint = packetInfo.endpoint;

		return packetInfo.packet;
	}
	return nullptr;
}

void BaseTransport::SendPacket(const udp::endpoint & endpoint, Packet* data)
{
	if (_sendQueue.size() <= MAX_SEND_QUEUE)
	{
		PacketInfo packetInfo;
		packetInfo.packet = data;
		packetInfo.endpoint = endpoint;
		_sendQueue.push(packetInfo);
	}
}

void BaseTransport::WritePackets()
{
	// Iterate through all send queue
	while (!_sendQueue.empty())
	{
		// Pop next packet to send
		PacketInfo packetInfo = _sendQueue.front();
		_sendQueue.pop();
		
		// Write packet to temporary buffer
		uint8_t* buffer = new uint8_t[max_packet_size_];
		memset(buffer, 0, sizeof(uint8_t)*max_packet_size_);

		int bytesUsed = WritePacket(&_context, packetInfo.packet, buffer, max_packet_size_);

		// Send packet to address
		InternalSendPacket(packetInfo.endpoint, buffer, bytesUsed);
		
		
		/*_debugData.createEntry(to_string(packetInfo.packet->GetType()) + " Sent " + std::to_string(bytesUsed) + "bytes to "
			+ packetInfo.endpoint.address().to_string(), NET_LOG);*/ 
		log.AddLog("[%s](->) packet(%d), bytes(%d)\n", "Log", packetInfo.packet->GetType(), (bytesUsed));
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

		if (_receiveQueue.size() >= MAX_RECEIVE_QUEUE)
			break;

		// All is good
		PacketInfo packetInfo;
		packetInfo.endpoint = endpoint;
		packetInfo.packet = ReadPacket(&_context, _packetFactory, buffer, bytesReceived);
		
		_receiveQueue.push(packetInfo);


		/*_debugData.createEntry(to_string(packetInfo.packet->GetType()) + " Received " + std::to_string(bytesReceived) + "bytes from "
			+ packetInfo.endpoint.address().to_string(), NET_LOG);*/
		log.AddLog("[%s](<-) packet(%d), bytes(%d)\n", "Log", packetInfo.packet->GetType(), (bytesReceived));
	}
}
