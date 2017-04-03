#ifndef _SNAPSHOTINTERPOLATIONSERVER_H_
#define _SNAPSHOTINTERPOLATIONSERVER_H_

#include "cocos2d.h"
#include "cocos-ext.h"

#include "asio\asio.hpp"

//#include "server.h"
//#include "NetworkDebugView.h"

#include "network_transport.h"
#include "network_connection.h"
#include "network_common.h"
#include "network_packet.h"

#include "SnapshotInterpolationSimulation.h"

#include "NetworkDebugDataSource.h"


#include "network_clientserver.h"

#include <stdio.h>

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