#include "network_packet.h"

#include "network_common.h"

Packet* PacketFactory::Create(uint32_t type)
{
	Packet* packet = CreatePacket(type);
	
	if (!packet)
		return nullptr;

	return packet;
}

void PacketFactory::SetPacketType(Packet* packet, uint32_t type)
{
	packet->SetType(type);
}

void PacketFactory::SetFactory(Packet * packet)
{
	packet->SetPacketFactory(*this);
}

bool Packet::Serialize(InStream & stream)
{
	return true;
}

bool Packet::Serialize(OStream & stream)
{
	return true;
}

int WritePacket(StreamContext* context, Packet * packet, void * buffer, int size)
{
	OStream stream{ (uint32_t*)buffer,size };
	stream.SetContext(context);
	
	uint32_t packetType = packet->GetType();

	// Write packet type to stream
	stream.SerializeInteger(packetType, 0, CLIENT_SERVER_MAX_PACKETS - 1);

	packet->SerializeInternal(stream);

	// Finished writing bits to stream
	stream.Flush();

	return stream.BytesUsed();
}

Packet * ReadPacket(StreamContext* context, PacketFactory* pf, void * buffer, int size)
{
	InStream stream{ (uint32_t*)buffer,size };
	stream.SetContext(context);

	uint32_t packetType;
	stream.SerializeInteger(packetType, 0, CLIENT_SERVER_MAX_PACKETS - 1);

	Packet* packet = pf->Create(packetType);
	packet->SerializeInternal(stream);

	return packet;
}
