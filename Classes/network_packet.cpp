#include "network_packet.h"

#include "network_common.h"

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

	stream.SerializeInteger(1, 0, 3);
	
	// Write packet type to stream
	stream.SerializeInteger(packetType);
	//stream.SerializeInteger(packetType, 0, CLIENT_SERVER_MAX_PACKETS - 1);

	packet->SerializeInternal(stream);

	// Finished writing bits to stream
	stream.Flush();

	return stream.BytesUsed();
}

Packet * ReadPacket(StreamContext* context, PacketFactory* pf, void * buffer, int size)
{
	InStream stream{ (uint32_t*)buffer,size };
	stream.SetContext(context);
	//stream.SetContext()
	uint32_t r;
	stream.SerializeInteger(r, 0, 3);

	uint32_t packetType;
	stream.SerializeInteger(packetType);
	//stream.SerializeInteger(packetType, 0, CLIENT_SERVER_MAX_PACKETS - 1);

	Packet* packet = pf->Create(packetType);
	packet->SerializeInternal(stream);

	return packet;
}
