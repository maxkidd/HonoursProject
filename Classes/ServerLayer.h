#ifndef _SERVERLAYER_H_
#define _SERVERLAYER_H_

#include "cocos2d.h"

#include "asio\asio.hpp"

#include "server.h"
//#include "NetworkDebugView.h"

class ServerLayer : public cocos2d::Layer
{
private:
	Server server;
	cocos2d::LabelTTF* _statusLabel;
	//NetworkDebugLayer* _networkDebug = nullptr;
public:
	ServerLayer();
	virtual ~ServerLayer();

	static cocos2d::Scene* scene();
	virtual bool init();
	CREATE_FUNC(ServerLayer);

	void ReloadScene();

	virtual void update(float dt);

protected:
	void createNetworkStatsLabel();

	void setupNetwork();
};
#endif