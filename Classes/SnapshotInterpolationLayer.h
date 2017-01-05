#ifndef _SNAPSHOTINTERPOLATIONLAYER_H_
#define _SNAPSHOTINTERPOLATIONLAYER_H_

#include "cocos2d.h"

#include "asio\asio.hpp"

#include "server.h"
//#include "NetworkDebugView.h"

#include "network_transport.h"

class SnapshotClient 
{


private:
	SocketTransport* _transport;
};

class SnapshotServer
{

private:
	SocketTransport* _transport;
};
class SnapshotInterpolationLayer : public cocos2d::Layer
{
private:
	Server server;
	cocos2d::LabelTTF* _statusLabel;
	//NetworkDebugLayer* _networkDebug = nullptr;
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

	void ConnectClient();
};
#endif