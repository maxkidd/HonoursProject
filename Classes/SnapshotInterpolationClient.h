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


#include "NetworkDebugDataSource.h"

#include <stdio.h>

class SnapshotClient
{
	enum ClientError
	{
		CLIENT_ERROR_NONE = 0,
		CLIENT_ERROR_REQUEST_DENIED
	} _error;

	enum ClientState
	{
		CLIENT_SLEEP, // Not active
		CLIENT_REQUESTING, // Requesting connection with server
		CLIENT_REQUEST_DENIED, // Request denied
		CLIENT_CONNECTED // Connected after request accepted
	};

private:
	SocketTransport* _transport = nullptr;

	SnapshotPacketFactory _packetFactory;

	bool _active = false;
	ClientState _state = CLIENT_SLEEP;

	const char* _serverIP;
	const char* _serverPort;
	udp::endpoint _serverEndpoint;

	Connection* _connection = nullptr;
public:
	SnapshotClient(NetworkDebugDataSource* _debugData);// TODO: File out stream for storage
	virtual ~SnapshotClient();

	void Init(const char* ip = "localhost", const char* port = "1500");

	bool Start();
	bool Stop();

	void Reset();

	// Send packets from the connection layer
	void SendPackets();

	// Receive packets to connection layer
	void ReceivePackets();

	// Transport layer
	void WritePackets();
	void ReadPackets();

	std::string GetNetworkState();

	bool IsActive() { return _active; }
protected:

	void ProcessPacket(Packet* packet, const udp::endpoint& endpoint);
	void ProcessAcceptPacket(ConnectionAcceptPacket* packet, const udp::endpoint& endpoint);
	void ProcessDeniedPacket(ConnectionDeniedPacket* packet, const udp::endpoint& endpoint);
	void ProcessConnectionPacket(ConnectionPacket* packet, const udp::endpoint& endpoint);
	void ProcessDisconnectPacket(ConnectionDisconnectPacket* packet, const udp::endpoint& endpoint);

	Packet* CreateRequestPacket();
	Packet* CreateConnectionPacket();

};

#endif