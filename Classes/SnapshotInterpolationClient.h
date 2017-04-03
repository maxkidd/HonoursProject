#ifndef _SNAPSHOTINTERPOLATIONCLIENT_H_
#define _SNAPSHOTINTERPOLATIONCLIENT_H_

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

class SnapshotClient : public Client
{
private:
	UnreliablePacketFactory _packetFactory;
	SnapshotMessageFactory _messageFactory;
public:
	SnapshotClient(NetworkSimulation* simulation, BaseTransport* transport);// TODO: File out stream for storage
	virtual ~SnapshotClient();

	// Process messages to the simulation layer
	virtual void ProcessMessages();
	virtual void GenerateMessages();

	// Overrides
	virtual PacketFactory* GetPacketFactory() { return &_packetFactory; };
	virtual MessageFactory* GetMessageFactory() { return &_messageFactory; };
};

#endif