#include "network_transport.h"
#include "network_common.h"

#include "ImGui\CCIMGUI.h"

#include "Logger.h"

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

SocketTransport::SocketTransport(PacketFactory * packetFactory, MessageFactory* messageFactory, unsigned short port) 
	: BaseTransport(packetFactory, messageFactory), _socket(_IOService, udp::endpoint(udp::v4(), port))
{
	_socket.non_blocking(true);


	/// ImGUI
	{
		CCIMGUI::getInstance()->addImGUI([=]() {

			NetworkLog::getInstance()->Draw("Network Log", false);

		}, std::string("Logging" + to_string(_socket.local_endpoint().port())));
	}

}

SocketTransport::~SocketTransport()
{
	CCIMGUI::getInstance()->removeImGUI(std::string("Logging" + to_string(_socket.local_endpoint().port())));
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
		

		/// ImGUI Debugging
		{
			uint32_t type = packetInfo.packet->GetType();
			NetworkLog::getInstance()->AddLog(LOG_PACKET_SENT, "[%s](-->) packet(%d), bytes(%d)\n", "Log", type, (bytesUsed));

			if (type == CLIENT_SERVER_PACKET_CONNECTION)
			{
				ConnectionPacket* conPacket = (ConnectionPacket*)packetInfo.packet;

				NetworkLog::getInstance()->AddLog(LOG_PACKET_ACK, "[%s](-->) SEQUENCE(%u), LATEST_ACK(%u), MISSED_SEQUENCE(%u)\n", "Log",
					conPacket->packetSequence, conPacket->ackReceipt, ~conPacket->prevAcks);

				NetworkLog::getInstance()->AddPacketLog(LOG_PACKET_SENT, true, bytesUsed);
			}
		}
	}
}

void BaseTransport::ReadPackets()
{
	while (true)
	{
		// Buffer to read to
		uint8_t* buffer = new uint8_t[max_packet_size_];
		memset(buffer, 0, sizeof(uint8_t)*max_packet_size_);

		// Recv endpoint
		asio::ip::basic_endpoint<udp> endpoint;

		// Receive packet
		int bytesReceived = InternalReceivePacket(endpoint, buffer, max_packet_size_);
		
		// Finish if no bytes received
		if (!bytesReceived)
			break;
		// Or receive queue is too big
		if (_receiveQueue.size() >= MAX_RECEIVE_QUEUE)
			break;

		// Create new PacketInfo entry
		PacketInfo packetInfo;
		packetInfo.endpoint = endpoint;
		packetInfo.packet = ReadPacket(&_context, _packetFactory, buffer, bytesReceived);
		
		_receiveQueue.push(packetInfo);

		/// ImGUI Debugging
		{
			uint32_t type = packetInfo.packet->GetType();
			NetworkLog::getInstance()->AddLog(LOG_PACKET_RECEIVED, "[%s](<--) packet(%d), bytes(%d)\n", "Log", type, (bytesReceived));


			if (type == CLIENT_SERVER_PACKET_CONNECTION)
			{
				ConnectionPacket* conPacket = (ConnectionPacket*)packetInfo.packet;

				NetworkLog::getInstance()->AddLog(LOG_PACKET_ACK, "[%s](<--) SEQUENCE(%u), LATEST_ACK(%u), MISSED_SEQUENCE(%u)\n", "Log",
					conPacket->packetSequence, conPacket->ackReceipt, ~conPacket->prevAcks);

				NetworkLog::getInstance()->AddPacketLog(LOG_PACKET_RECEIVED, false, bytesReceived);
			}
		}
	}
}
