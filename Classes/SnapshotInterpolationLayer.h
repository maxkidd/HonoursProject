#ifndef _SNAPSHOTINTERPOLATIONLAYER_H_
#define _SNAPSHOTINTERPOLATIONLAYER_H_

#include "cocos2d.h"
#include "cocos-ext.h"

#include "asio\asio.hpp"

#include "network_transport.h"
#include "network_connection.h"
#include "network_packet.h"

#include <stdio.h>

#include "SnapshotInterpolationClient.h"
#include "SnapshotInterpolationServer.h"
#include "SnapshotInterpolationSimulation.h"
#include "GameLayer.h"

/**
	Snapshot Interpolation Layer
	- Layer for snapshot interpolation technique
	- Calls client or server update functions
	- Displays cocos2d ui
	- Passes the transport protocol to client or server
*/
class SnapshotInterpolationLayer : public GameLayer
{
private:
	SnapshotServer* server = nullptr;
	SnapshotClient* client = nullptr;

	SnapshotInterpolationSimulation* _simulation;
	cocos2d::LabelTTF* _statusLabel;

	SocketTransport _transport;

	TableView* _tableView = nullptr;

	float _networkTimer = 0.0f;
	float _physicsTimer = 0.0f;

public:
	SnapshotInterpolationLayer();
	virtual ~SnapshotInterpolationLayer();

	static cocos2d::Scene* scene();
	virtual bool init();
	CREATE_FUNC(SnapshotInterpolationLayer);

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