#include "network_connection.h"

#include "network_common.h"



Connection::Connection(udp::endpoint endpoint, PacketFactory & pf, MessageFactory * mf)
	: _endpoint(endpoint), _packetFactory(&pf), _messageFactory(mf)
{
	_channel = new Channel(mf);
	_lastProcessedPacket = std::chrono::high_resolution_clock::now();

	sentAckEntries = new bool[sequenceBufferSize] {false};
	recvAckEntries = new bool[sequenceBufferSize] {false};

}

Connection::~Connection()
{
	delete _channel;

	delete[] sentAckEntries;
	delete[] recvAckEntries;
}

bool Connection::HasTimedOut()
{
	std::chrono::duration<float> elapsed = std::chrono::high_resolution_clock::now() - _lastProcessedPacket;
	if (elapsed.count() > TIMEOUT)
		return true;
	return false;
}

ConnectionPacket * Connection::GeneratePacket()
{
	ConnectionPacket* packet = (ConnectionPacket*)_packetFactory->Create(CLIENT_SERVER_PACKET_CONNECTION);

	
	packet->packetSequence = sentSequence;

	// Generate ack bits
	{
		uint32_t ack = recvSequence - 1;
		uint32_t ackBits = 0;

		uint32_t mask = 1; // 1 bit mask

		for (int i = 0; i < 32; ++i)
		{
			uint32_t sequence = ack - 1;
			if (recvAckEntries[sequence % sequenceBufferSize])
			{
				ackBits |= mask;
			}
			mask <<= 1; // shift to the next bit
		}

		packet->ackReceipt = ack;
		packet->prevAcks = ackBits;
	}


	// Insert ack packet entry
	{
		uint32_t seq = packet->packetSequence;
		if (seq + 1 > sentSequence)
		{
			sentSequence = seq + 1;
		}
		sentAckEntries[seq % sequenceBufferSize] = false;
	}

	// Add messages to connection packet from channel layer
	ChannelPacket* channel_packet = new ChannelPacket();
	_channel->GetPacketData(*channel_packet, 0);

	memcpy(&packet->_channelEntry, &channel_packet, sizeof(ChannelPacket*));
	memcpy(&packet->_messageFactory, &_messageFactory, sizeof(MessageFactory*));

	return packet;
}

bool Connection::ProcessPacket(ConnectionPacket * packet)
{
	// Chrono timepoint
	_lastProcessedPacket = std::chrono::high_resolution_clock::now();


	// Process sequence
	uint32_t seq = packet->packetSequence;
	if (seq + 1 > recvSequence)
	{
		recvSequence = seq + 1;
	}
	recvAckEntries[seq % sequenceBufferSize] = true;


	// Process receipts
	ProcessAcks(packet->ackReceipt, packet->prevAcks);

	_channel->ProcessPacketData(*(packet->_channelEntry));

	return true;
}

void Connection::ProcessAcks(uint32_t ack, uint32_t prevAcks)
{
	for (int i = 0; i < 32; ++i)
	{
		// If previous ack arrived
		if (prevAcks & 1)
		{
			const uint32_t sequence = ack - i;// Current sequence number to test

			sentAckEntries[sequence % sequenceBufferSize] = true;

		}

		prevAcks >>= 1; // shift to test the next ack bit
	}
}

void Connection::SendMsg(NMessage * message)
{
	_channel->SendMsg(message);
}

NMessage * Connection::ReceiveMsg()
{
	return _channel->ReceiveMsg();
}

udp::endpoint Connection::Endpoint()
{
	return _endpoint;
}
