#include "ServerLayer.h"

#include <thread>
#include <utility>

using namespace std;
using namespace cocos2d;

ServerLayer::ServerLayer() : server(std::atoi("1500")), _statusLabel(nullptr)
{
}

ServerLayer::~ServerLayer()
{
	unscheduleUpdate();

	if (_statusLabel)
	{
		_statusLabel = nullptr;
	}
}

cocos2d::Scene * ServerLayer::scene()
{
	auto scene = Scene::create();
	ServerLayer* layer = ServerLayer::create();
	scene->addChild(layer);

	return scene;
}

bool ServerLayer::init()
{
	Layer::init();


	// Menu
	MenuItemFont::setFontSize(16);
	MenuItemFont::setFontName("fonts/UbuntuMono-R.ttf");

	auto reloadItem = MenuItemFont::create("Reload Scene", CC_CALLBACK_0(ServerLayer::ReloadScene, this));

	Menu* menu2 = Menu::create(reloadItem, NULL);
	menu2->alignItemsVertically();
	addChild(menu2);


	createNetworkStatsLabel();

	std::thread bt(std::bind(&ServerLayer::setupNetwork, this));
	bt.detach();

	scheduleUpdate();

	return true;
}

void ServerLayer::ReloadScene()
{
	Director::getInstance()->replaceScene(ServerLayer::scene());
}

void ServerLayer::update(float dt)
{
	if (server.received)
		_statusLabel->setString(std::string("Connecting to...").append(server.address));
	if (server.message == Message::NET_NULL)
	{
		_statusLabel->setString(std::string("NULL message from...").append(server.address));
	}
	else if (server.message == Message::NET_PING)
	{
		_statusLabel->setString(std::string("PING message from...").append(server.address));
	}
}

void ServerLayer::createNetworkStatsLabel()
{
	Size winSize = Director::getInstance()->getWinSize();

	_statusLabel = LabelTTF::create("Not Connected", "Ariel", 12, winSize, TextHAlignment::RIGHT, TextVAlignment::BOTTOM);

	addChild(_statusLabel);

	_statusLabel->setScale(1 / Director::getInstance()->getContentScaleFactor());

	const int height_spacing = 22 / Director::getInstance()->getContentScaleFactor();
	_statusLabel->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f + height_spacing * 0) + Director::getInstance()->getVisibleOrigin());
}

void ServerLayer::setupNetwork()
{

	try
	{

		//server s(std::atoi("1500"));

		server.run();
	}
	catch (std::exception& e)
	{

	}
}
