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
	return false;
}

bool Packet::Serialize(OStream & stream)
{
	return false;
}
