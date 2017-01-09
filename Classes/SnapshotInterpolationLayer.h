#ifndef _SNAPSHOTINTERPOLATIONLAYER_H_
#define _SNAPSHOTINTERPOLATIONLAYER_H_

#include "cocos2d.h"

#include "asio\asio.hpp"

#include "server.h"
//#include "NetworkDebugView.h"

#include "network_transport.h"
#include "network_connection.h"
#include "network_config.h"

class SnapshotClient 
{
public:
	SnapshotClient();
	virtual ~SnapshotClient();

	void Init(const char* ip = "localhost", const char* port = "1500");

	bool Start();
	bool Stop();

	void Reset();

	bool IsActive() { return _active; }
private:
	SocketTransport* _transport;

	bool _active = false;

	const char* _serverIP;
	const char* _serverPort;

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

	bool IsActive() { return _active; }
private:
	SocketTransport* _transport;

	bool _active = false;

	const char* _serverIP;
	const char* _serverPort;

	int _numClients;
	Connection* _connection[MAX_CLIENTS];

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

	void setupNetwork();

	void ConnectAsClient();

};
#endif