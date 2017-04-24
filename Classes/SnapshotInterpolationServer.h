#ifndef _SNAPSHOTINTERPOLATIONSERVER_H_
#define _SNAPSHOTINTERPOLATIONSERVER_H_

#include "network_clientserver.h"


//	Snapshot Server
class SnapshotServer : public Server
{
private:
	UnreliablePacketFactory _packetFactory;
	SnapshotMessageFactory _messageFactory;

public:
	SnapshotServer(NetworkSimulation* simulation, BaseTransport* transport);
	virtual ~SnapshotServer();

	virtual void ProcessMessages();
	virtual void GenerateMessages();

	virtual PacketFactory* GetPacketFactory() { return &_packetFactory; };
	virtual MessageFactory* GetMessageFactory() { return &_messageFactory; };

};


#endif