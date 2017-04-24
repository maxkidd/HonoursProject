#ifndef _STATESYNCSERVER_H_
#define _STATESYNCSERVER_H_

#include "network_clientserver.h"

//	State Synchronization Server
class StateSyncServer : public Server
{
private:
	UnreliablePacketFactory _packetFactory;
	StateSyncMessageFactory _messageFactory;

public:
	StateSyncServer(NetworkSimulation* simulation, BaseTransport* transport);
	virtual ~StateSyncServer();

	virtual void ProcessMessages();
	virtual void GenerateMessages();

	virtual PacketFactory* GetPacketFactory() { return &_packetFactory; };
	virtual MessageFactory* GetMessageFactory() { return &_messageFactory; };
	

protected:


};


#endif