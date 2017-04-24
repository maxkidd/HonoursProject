#ifndef _SNAPSHOTINTERPOLATIONCLIENT_H_
#define _SNAPSHOTINTERPOLATIONCLIENT_H_

#include "network_clientserver.h"


//	Snapshot Client
class SnapshotClient : public Client
{
private:
	UnreliablePacketFactory _packetFactory;
	SnapshotMessageFactory _messageFactory;

public:
	SnapshotClient(NetworkSimulation* simulation, BaseTransport* transport);
	virtual ~SnapshotClient();

	// Process messages to the simulation layer
	virtual void ProcessMessages();
	virtual void GenerateMessages();

	// Overrides
	virtual PacketFactory* GetPacketFactory() { return &_packetFactory; };
	virtual MessageFactory* GetMessageFactory() { return &_messageFactory; };
};

#endif