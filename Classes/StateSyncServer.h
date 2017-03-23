#ifndef _STATESYNCSERVER_H_
#define _STATESYNCSERVER_H_

#include "cocos2d.h"
#include "cocos-ext.h"

#include "asio\asio.hpp"

//#include "server.h"
//#include "NetworkDebugView.h"

#include "network_transport.h"
#include "network_connection.h"
#include "network_common.h"
#include "network_packet.h"


#include "network_clientserver.h"

#include "StateSyncSimulation.h"

#include "NetworkDebugDataSource.h"

#include <stdio.h>

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