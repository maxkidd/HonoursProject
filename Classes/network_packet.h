#ifndef _NETWORKPACKET_H_
#define _NETWORKPACKET_H_

#include "network_serializable.h"

#define MAX_PACKET_SIZE 1024;

class PacketFactory;

class Packet : public Serializable
{
	Packet() : _factory(nullptr), _type(0){}
protected:
	friend class PacketFactory;

	void SetType(int type) { _type = type; }
	void SetPacketFactory(PacketFactory& factory) { _factory = &factory; }

private:
	PacketFactory* _factory;
	int _type;
};

class PacketFactory
{
public:
	PacketFactory(int packetTypes) : _packetTypes(packetTypes) {}
	virtual ~PacketFactory() {}

	Packet* Create(int type);

protected:
	void SetPacketType(Packet* packet, int type);
	void SetFactory(Packet* packet);

	virtual Packet* CreatePacket(int type) { (void)type; return nullptr; }
private:
	int _packetTypes;
};

#define PACKET_FACTORY_START(factory_class, base_class, num_packets)	\
class factory_class : public base_class									\
{																		\
public:																	\
	factory_class(num_packets) : base_class(num_packets) {}				\
																		\
	Packet* CreatePacket(int type)										\
	{																	\
		Packet* packet = base_class::CreatePacket(type);				\
		switch (type){																

#define PACKET_FACTORY_TYPE(packet_type, packet_class)							\
		case packet_type:														\
			packet = new packet_class();										\
			if (!packet)														\
				return nullptr;													\
			SetPacketType(packet, packet_type);									\
			SetPacketFactory(packet)											\
			return packet;

#define PACKET_FACTORY_END()			\
			default: return nullptr;	\
		}								\
	}									\
};										

#endif