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
		CLIENT_SLEEP,
		CLIENT_REQUESTING,
		CLIENT_REQUEST_DENIED,
		CLIENT_CONNECTED
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

	bool IsActive() { return _active; }
protected:

	//void ProcessPacket(Packet* packet, const udp::endpoint& endpoint);
	//void ProcessAcceptPacket(ConnectionAcceptPacket* packet, const udp::endpoint& endpoint);
	//void ProcessDeniedPacket(ConnectionDeniedPacket* packet, const udp::endpoint& endpoint);
	//void ProcessConnectionPacket(ConnectionPacket* packet, const udp::endpoint& endpoint);
	//void ProcessDisconnectPacket(ConnectionDisconnectPacket* packet, const udp::endpoint& endpoint);

	Packet* CreateRequestPacket();
	Packet* CreateConnectionPacket();
private:
	SocketTransport* _transport;

	SnapshotPacketFactory _packetFactory;

	bool _active = false;
	ClientState _state;

	const char* _serverIP;
	const char* _serverPort;
	udp::endpoint _serverEndpoint;

	Connection* _connection;

};

class SnapshotServer
{
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

	bool IsActive() { return _active; }
protected:

	void ProcessPacket(Packet* packet, udp::endpoint endpoint);
	/* Process connection request packet from client */
	void ProcessRequestPacket(ConnectionRequestPacket* packet, const udp::endpoint& endpoint);
	void ProcessConnectionPacket(ConnectionPacket* packet, const udp::endpoint& endpoint);
	void ProcessDisconnectPacket(ConnectionDisconnectPacket* packet, const udp::endpoint& endpoint);

	void SendPacketToClient(uint16_t clientID, Packet* packet);

private:
	SocketTransport* _transport;

	SnapshotPacketFactory _packetFactory;

	bool _active = false;

	const char* _serverIP;
	const char* _serverPort;

	static const int _maxSlots = 32;
	//std::vector<Connection> _connections; // 32 connection slots

	// Clients
	bool _clientConnected[_maxSlots];
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