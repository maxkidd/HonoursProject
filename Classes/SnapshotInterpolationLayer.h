#ifndef _SNAPSHOTINTERPOLATIONLAYER_H_
#define _SNAPSHOTINTERPOLATIONLAYER_H_

#include "cocos2d.h"

#include "asio\asio.hpp"

//#include "server.h"
//#include "NetworkDebugView.h"

#include "network_transport.h"
#include "network_connection.h"
#include "network_common.h"
#include "network_packet.h"

#include <stdio.h>

struct ClientData
{

};

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

public:
	SnapshotClient();
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
private:
	SocketTransport* _transport = nullptr;

	SnapshotPacketFactory _packetFactory;

	bool _active = false;
	ClientState _state = CLIENT_SLEEP;

	const char* _serverIP;
	const char* _serverPort;
	udp::endpoint _serverEndpoint;

	Connection* _connection = nullptr;

};

class SnapshotServer
{

	enum ServerState
	{
		SERVER_SLEEP, // Not active
		SERVER_ALIVE, // Running and can accept clients
		SERVER_CONNECTED // Running with 1 or more clients connected
	};

public:
	SnapshotServer();
	virtual ~SnapshotServer();

	void Init(char* port = "1500");

	bool Start();
	bool Stop();

	void Reset();

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

private:
	SocketTransport* _transport = nullptr;

	SnapshotPacketFactory _packetFactory;

	ServerState _state = SERVER_SLEEP;

	bool _active = false;

	const char* _serverIP;
	const char* _serverPort;

	static const int _maxSlots = 32;
	//std::vector<Connection> _connections; // 32 connection slots


	uint16_t _connectedClients = 0;
	// Clients
	bool _clientConnected[_maxSlots] = {false};
	//uint16_t _clientID[_maxSlots];
	ClientData _clientData[_maxSlots];
	Connection* _connections[_maxSlots];


};
class SnapshotInterpolationLayer : public cocos2d::Layer
{
private:
	SnapshotServer* server;
	SnapshotClient* client;

	cocos2d::LabelTTF* _statusLabel;

	float _networkTimer = 0.0f;

public:
	SnapshotInterpolationLayer();
	virtual ~SnapshotInterpolationLayer();

	static cocos2d::Scene* scene();
	virtual bool init();
	CREATE_FUNC(SnapshotInterpolationLayer);

	void ReloadScene();

	virtual void update(float dt);

protected:
	void createNetworkStatsLabel();

	void setupServer();

	void connectAsClient();
	void connectAsServer();

};
#endif