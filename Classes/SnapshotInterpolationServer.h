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

#include <stdio.h>

class SnapshotServer
{

	struct ClientData
	{

	};

	enum ServerState
	{
		SERVER_SLEEP, // Not active
		SERVER_ALIVE, // Running and can accept clients
		SERVER_CONNECTED // Running with 1 or more clients connected
	};

private:
	S_SnapshotInterpolationSimulation* _simulation;

	SocketTransport* _transport = nullptr;

	SnapshotPacketFactory _packetFactory;
	SnapshotMessageFactory _messageFactory;

	ServerState _state = SERVER_SLEEP;

	bool _active = false;

	static const int MAX_SLOTS = 32;
	static const int NULL_CLIENT_ID = MAX_SLOTS;
	uint16_t _connectedClients = 0;
	
	// Clients
	bool _clientConnected[MAX_SLOTS] = { false };
	ClientData _clientData[MAX_SLOTS];
	Connection* _connections[MAX_SLOTS];
public:
	SnapshotServer(NetworkDebugDataSource* debugData, S_SnapshotInterpolationSimulation* simulation);
	virtual ~SnapshotServer();

	bool Start();
	bool Stop();

	void Reset();

	void GenerateSnapshots();

	void SendPackets();
	void ReceivePackets();

	// Transport layer
	void WritePackets();
	void ReadPackets();

	std::string GetNetworkState();

	bool IsActive() { return _active; }
protected:

	void ProcessPacket(Packet* packet, udp::endpoint endpoint);
	/* Process connection request packet from client */
	void ProcessRequestPacket(ConnectionRequestPacket* packet, const udp::endpoint& endpoint);
	void ProcessConnectionPacket(ConnectionPacket* packet, const udp::endpoint& endpoint);
	void ProcessDisconnectPacket(ConnectionDisconnectPacket* packet, const udp::endpoint& endpoint);

	void SendPacketToClient(uint16_t clientID, Packet* packet);

};


#endif