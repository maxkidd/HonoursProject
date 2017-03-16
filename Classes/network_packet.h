#ifndef _NETWORK_PACKET_H_
#define _NETWORK_PACKET_H_

#include "network_serializable.h"

#define MAX_PACKET_SIZE 1024;

class PacketFactory;

class Packet : public Serializable
{
public:
	Packet() : _factory(nullptr), _type(0){}

	uint32_t GetType() { return _type; }
protected:
	friend class PacketFactory;

	void SetType(uint32_t type) { _type = type; }
	void SetPacketFactory(PacketFactory& factory) { _factory = &factory; }

	virtual bool Serialize(InStream& stream);
	virtual bool Serialize(OStream& stream);
private:
	PacketFactory* _factory;
	uint32_t _type;
};

class PacketFactory
{
public:
	PacketFactory(int packetTypes) : _packetTypes(packetTypes) {}
	virtual ~PacketFactory() {}

	Packet* Create(uint32_t type);

protected:
	void SetPacketType(Packet* packet, uint32_t type);
	void SetFactory(Packet* packet);

	virtual Packet* CreatePacket(uint32_t type) { (void)type; return nullptr; }
private:
	uint32_t _packetTypes;
};

#define PACKET_FACTORY_START(factory_class, base_class, num_packets)				\
class factory_class : public base_class{											\
public:																				\
	factory_class(int packetTypes = num_packets) : base_class(packetTypes) {}		\
	virtual Packet* CreatePacket(uint32_t type){													\
		Packet* packet = base_class::CreatePacket(type);							\
		switch (type){

#define PACKET_FACTORY_TYPE(packet_type, packet_class)							\
		case packet_type:														\
			packet = new packet_class();										\
			if (!packet) return nullptr;										\
			SetPacketType(packet, packet_type);									\
			SetFactory(packet);													\
			return packet;

#define PACKET_FACTORY_END()			\
			default: return nullptr;	\
		}								\
	}									\
};


// Write packet to buffer
int WritePacket(StreamContext* context, Packet* packet, void* buffer, int size);

// Read packet from buffer
Packet* ReadPacket(StreamContext* context, PacketFactory* pf, void* buffer, int size);


#endif