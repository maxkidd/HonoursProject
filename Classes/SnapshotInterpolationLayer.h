#ifndef _SNAPSHOTINTERPOLATIONLAYER_H_
#define _SNAPSHOTINTERPOLATIONLAYER_H_

#include "cocos2d.h"
#include "cocos-ext.h"

#include "asio\asio.hpp"

//#include "server.h"
//#include "NetworkDebugView.h"

#include "network_transport.h"
#include "network_connection.h"
//#include "network_common.h"
#include "network_packet.h"


#include "NetworkDebugDataSource.h"

#include <stdio.h>

#include "SnapshotInterpolationClient.h"
#include "SnapshotInterpolationServer.h"

class SnapshotInterpolationLayer : public cocos2d::Layer
{
private:
	SnapshotServer* server;
	SnapshotClient* client;

	cocos2d::LabelTTF* _statusLabel;

	NetworkDebugDataSource _netDebugData;
	TableView* _tableView = nullptr;

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