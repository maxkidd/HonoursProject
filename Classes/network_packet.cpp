#include "network_packet.h"

Packet* PacketFactory::Create(int type)
{
	Packet* packet = CreatePacket(type);
	
	if (!packet)
		return nullptr;

	return packet;
}

void PacketFactory::SetPacketType(Packet* packet, int type)
{
	packet->SetType(type);
}

void PacketFactory::SetFactory(Packet * packet)
{
	packet->SetPacketFactory(*this);
}

bool Packet::Serialize(IStream & stream)
{
	return true;
}

bool Packet::Serialize(OStream & stream)
{
	return true;
}

bool WritePacket(Packet * packet, void * buffer, int size)
{
	OStream stream{ (uint32_t*)buffer,size };
	
	int packetType = packet->GetType();
	
	// Write packet type to stream
	stream.SerializeInteger(packetType);

	// Finished writing bits to stream
	stream.Flush();

	return true;
}

Packet * ReadPacket(void * buffer, int size)
{
	InStream stream{ (uint32_t*)buffer,size };

	uint32_t test;

	stream.SerializeInteger(test);

	return nullptr;
}
