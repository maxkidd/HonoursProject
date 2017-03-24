#ifndef _STATESYNCLAYER_H_
#define _STATESYNCLAYER_H_

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

#include "StateSyncClient.h"
#include "StateSyncServer.h"
#include "StateSyncSimulation.h"

class StateSyncLayer : public cocos2d::Layer
{
private:
	StateSyncServer* server = nullptr;
	StateSyncClient* client = nullptr;
	StateSyncSimulation* _simulation = nullptr;
	cocos2d::LabelTTF* _statusLabel = nullptr;

	SocketTransport _transport;

	NetworkDebugDataSource* _netDebugData = nullptr;
	TableView* _tableView = nullptr;

	float _networkTimer = 0.0f;
	float _physicsTimer = 0.0f;

public:
	StateSyncLayer();
	virtual ~StateSyncLayer();

	static cocos2d::Scene* scene();
	virtual bool init();
	CREATE_FUNC(StateSyncLayer);

	void ReloadScene();

	virtual void update(float dt);

	//    virtual void registerWithTouchDispatcher();
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)override;
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)override;
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)override;

protected:
	void createNetworkStatsLabel();

	void setupServer();

	void connectAsClient();
	void connectAsServer();

};
#endif