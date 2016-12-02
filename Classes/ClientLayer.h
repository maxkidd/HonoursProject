#ifndef _CLIENTLAYER_H_
#define _CLIENTLAYER_H_

#include "cocos2d.h"

#include "asio\asio.hpp"
#include "Client.h"

#include "cocos2d\cocos\ui\UIEditBox\UIEditBox.h"

using namespace cocos2d;
class ClientLayer : public cocos2d::Layer, public ui::EditBoxDelegate
{
private:
	cocos2d::LabelTTF* _statusLabel = nullptr;

	Client client;
public:
	ClientLayer();
	virtual ~ClientLayer();

	static cocos2d::Scene* scene();
	virtual bool init();
	CREATE_FUNC(ClientLayer);

	void ReloadScene();
	void SendMessageToServer();

	virtual void update(float dt);

	// EditBoxDelegateInherit
	void editBoxReturn(ui::EditBox* editBox);

protected:
	void createNetworkStatsLabel();

	void setupNetwork();
};
#endif