#include "ClientLayer.h"

#include "asio\asio.hpp"

using asio::ip::udp;
using namespace std;
using namespace cocos2d;

ClientLayer::ClientLayer() :
	client(std::atoi("1500"))
{
}

ClientLayer::~ClientLayer()
{
	unscheduleUpdate();
}

cocos2d::Scene * ClientLayer::scene()
{
	auto scene = Scene::create();
	ClientLayer* layer = ClientLayer::create();
	scene->addChild(layer);

	return scene;
}

bool ClientLayer::init()
{
	Layer::init();


	// Menu
	MenuItemFont::setFontSize(16);
	MenuItemFont::setFontName("fonts/UbuntuMono-R.ttf");

	auto reloadItem = MenuItemFont::create("Reload Scene", CC_CALLBACK_0(ClientLayer::ReloadScene, this));
	auto messageItem = MenuItemFont::create("Send Message", CC_CALLBACK_0(ClientLayer::SendMessageToServer, this));

	Menu* menu2 = Menu::create(reloadItem, messageItem, NULL);
	menu2->alignItemsVertically();
	addChild(menu2);


	createNetworkStatsLabel();

	std::thread bt(std::bind(&ClientLayer::setupNetwork, this));
	bt.detach();

	scheduleUpdate();

	return true;
}
void ClientLayer::SendMessageToServer()
{
	Size editBoxSize = Size(200.0f, 20.0f);
	Vec2 visibleOrigin = Director::getInstance()->getVisibleOrigin();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	ui::EditBox* _editName; 
	std::string pNormalSprite = "HelloWorld.png";
	_editName = ui::EditBox::create(editBoxSize, ui::Scale9Sprite::create(pNormalSprite));
	_editName->setPosition(Vec2(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height * 3 / 4));
	_editName->setFontName("Paint Boy");
	_editName->setFontSize(25);
	_editName->setFontColor(Color3B::RED);
	_editName->setPlaceHolder("");
	_editName->setPlaceholderFontColor(Color3B::WHITE);
	_editName->setMaxLength(8);
	_editName->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	_editName->setDelegate(this);
	addChild(_editName);
}

void ClientLayer::ReloadScene()
{
	Director::getInstance()->replaceScene(ClientLayer::scene());
}

void ClientLayer::update(float dt)
{
	//client.update();

	if (client.sent)
		_statusLabel->setString(std::string("Initiating contact to... ").append(client.address));
	if(client.received)
		_statusLabel->setString(std::string("Connecting to... ").append(client.address));
	if (client.message == Message::NET_NULL)
	{
		_statusLabel->setString(std::string("NULL message from...").append(client.address));
	}
	else if (client.message == Message::NET_PING)
	{
		_statusLabel->setString(std::string("Latency is... ").append(to_string(client.latency)).append(" milliseconds"));
	}

}

void ClientLayer::editBoxReturn(ui::EditBox * editBox)
{
	const char * message = editBox->getText();
	client.sendMessage(message, std::string(message).size());
}

void ClientLayer::createNetworkStatsLabel()
{
	Size winSize = Director::getInstance()->getWinSize();

	_statusLabel = LabelTTF::create("Not Connected", "Ariel", 12, winSize,TextHAlignment::RIGHT, TextVAlignment::BOTTOM);

	addChild(_statusLabel);

	_statusLabel->setScale(1 / Director::getInstance()->getContentScaleFactor());

	const int height_spacing = 22 / Director::getInstance()->getContentScaleFactor();
	_statusLabel->setPosition(Vec2(winSize.width/2.0f, winSize.height / 2.0f + height_spacing * 0) + Director::getInstance()->getVisibleOrigin());
}

void ClientLayer::setupNetwork()
{
	try
	{
		client.initConnection();
		client.run();
	}
	catch (std::exception& e)
	{

	}
}
