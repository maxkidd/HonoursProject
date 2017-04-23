#ifndef _STATESYNCLAYER_H_
#define _STATESYNCLAYER_H_

#include "cocos2d.h"
#include "cocos-ext.h"

#include "asio\asio.hpp"

#include "network_transport.h"
#include "network_connection.h"
#include "network_packet.h"

#include <stdio.h>

#include "StateSyncClient.h"
#include "StateSyncServer.h"
#include "StateSyncSimulation.h"

#include "GameLayer.h"

/**
	State Synchronization Layer
	- Layer for state synchronization technique
	- Calls client or server update functions
	- Displays cocos2d ui
	- Passes the transport protocol to client or server
*/
class StateSyncLayer : public GameLayer
{
private:
	StateSyncServer* server = nullptr;
	StateSyncClient* client = nullptr;
	StateSyncSimulation* _simulation = nullptr;
	cocos2d::LabelTTF* _statusLabel = nullptr;

	SocketTransport _transport;


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