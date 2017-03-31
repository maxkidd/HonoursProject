#ifndef _STATESYNCCLIENT_H_
#define _STATESYNCCLIENT_H_

#include "network_clientserver.h"

class StateSyncClient : public Client
{
private:
	UnreliablePacketFactory _packetFactory;
	StateSyncMessageFactory _messageFactory;

public:
	StateSyncClient(NetworkSimulation* simulation, BaseTransport* transport);// TODO: File out stream for storage
	virtual ~StateSyncClient();

	// Process messages to the simulation layer
	virtual void ProcessMessages();
	virtual void GenerateMessages();

	// Overrides
	virtual PacketFactory* GetPacketFactory() { return &_packetFactory; };
	virtual MessageFactory* GetMessageFactory() { return &_messageFactory; };


};

#endif