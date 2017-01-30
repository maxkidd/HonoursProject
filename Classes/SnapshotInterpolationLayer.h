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

#define MAX_CLIENTS 32;

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
	//void WritePackets();
	//void ReadPackets();

	bool IsActive() { return _active; }
protected:
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
	//void WritePackets();
	//void ReadPackets();

	bool IsActive() { return _active; }
protected:

private:
	SocketTransport* _transport;

	SnapshotPacketFactory _packetFactory;

	bool _active = false;

	const char* _serverIP;
	const char* _serverPort;

	int _numClients;
	Connection* _connections; // MAX 32

};
class SnapshotInterpolationLayer : public cocos2d::Layer
{
private:
	SnapshotServer server;
	SnapshotClient client;

	cocos2d::LabelTTF* _statusLabel;
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

};
#endif