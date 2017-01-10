#include "SnapshotInterpolationLayer.h"

#include <thread>
#include <utility>

#include "cocos\ui\UITextField.h"
#include "cocos\ui\UIButton.h"
#include "cocos\ui\UIWidget.h"

using namespace std;
using namespace cocos2d;

SnapshotInterpolationLayer::SnapshotInterpolationLayer() : _statusLabel(nullptr)
{
}

SnapshotInterpolationLayer::~SnapshotInterpolationLayer()
{
	unscheduleUpdate();

	if (_statusLabel)
	{
		_statusLabel = nullptr;
	}
}

cocos2d::Scene * SnapshotInterpolationLayer::scene()
{
	auto scene = Scene::create();
	SnapshotInterpolationLayer* layer = SnapshotInterpolationLayer::create();
	scene->addChild(layer);

	return scene;
}

bool SnapshotInterpolationLayer::init()
{
	Layer::init();

	Size winSize = Director::getInstance()->getWinSize();

	// Menu
	MenuItemFont::setFontSize(16);
	MenuItemFont::setFontName("fonts/UbuntuMono-R.ttf");

	//auto reloadItem = MenuItemFont::create("Reload Scene", CC_CALLBACK_0(SnapshotInterpolationLayer::ReloadScene, this));

	//Menu* menu2 = Menu::create(reloadItem, NULL);
	//menu2->alignItemsVertically();
	//addChild(menu2);


	auto connectItem = MenuItemFont::create("Connect to Server", CC_CALLBACK_0(SnapshotInterpolationLayer::connectAsClient, this));
	
	Menu* menu3 = Menu::create(connectItem, NULL);
	menu3->alignItemsHorizontally();
	menu3->setScale(1 / Director::getInstance()->getContentScaleFactor());
	menu3->setPosition(Vec2(winSize.width / 2.0f, winSize.height - 10.0f) + Director::getInstance()->getVisibleOrigin());

	addChild(menu3);

	createNetworkStatsLabel();

	std::thread bt(std::bind(&SnapshotInterpolationLayer::setupNetwork, this));
	bt.detach();

	scheduleUpdate();

	return true;
}

void SnapshotInterpolationLayer::ReloadScene()
{
	Director::getInstance()->replaceScene(SnapshotInterpolationLayer::scene());
}

void SnapshotInterpolationLayer::update(float dt)
{
	std::string debugString;
	if (true) // Client or Server tick
	{
		if (server.IsActive()) // Server
		{
			server.SendPackets();
			//server.WritePackets();
			//server.ReadPackets();
			//server.ReceivePackets();

			debugString.append("Server: ");
		}
		else if (client.IsActive()) // Client
		{
			debugString.append("Client: ");
		}
		else
		{
			// Start server

			debugString.append("No Client/Server running");
		}
	}

	_statusLabel->setString(debugString);
}

void SnapshotInterpolationLayer::createNetworkStatsLabel()
{
	Size winSize = Director::getInstance()->getWinSize();

	_statusLabel = LabelTTF::create("Not Connected", "Ariel", 12, winSize, TextHAlignment::RIGHT, TextVAlignment::BOTTOM);

	addChild(_statusLabel);

	_statusLabel->setScale(1 / Director::getInstance()->getContentScaleFactor());

	const int height_spacing = 22 / Director::getInstance()->getContentScaleFactor();
	_statusLabel->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f + height_spacing * 0) + Director::getInstance()->getVisibleOrigin());
}

void SnapshotInterpolationLayer::setupNetwork()
{

	try
	{

		//server s(std::atoi("1500"));

		//server.run();
	}
	catch (std::exception& e)
	{

	}
}

void SnapshotInterpolationLayer::connectAsClient()
{
	Size winSize = Director::getInstance()->getWinSize();

	Node *connectNode = Node::create();

	// IP address field
	ui::TextField *ipText = ui::TextField::create("IP ADDRESS", "Ariel", 50);
	ipText->setMaxLength(12);
	ipText->setMaxLengthEnabled(true);
	ipText->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f  + 30.0f) + Director::getInstance()->getVisibleOrigin());
	connectNode->addChild(ipText);

	// Port address field
	ui::TextField *portText = ui::TextField::create("PORT", "Ariel", 50);
	portText->setMaxLength(12);
	portText->setMaxLengthEnabled(true);
	portText->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f - 20.0f) + Director::getInstance()->getVisibleOrigin());
	connectNode->addChild(portText);

	// Connect button
	ui::Button *connectButton = ui::Button::create(
		"CloseNormal.png");
	connectButton->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f - 60.0f) + Director::getInstance()->getVisibleOrigin());

	connectButton->addTouchEventListener([this, connectNode, ipText, portText](Ref* pSender, ui::Widget::TouchEventType type)
	{
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			if (server.IsActive())
			{

			}

			client.Init(ipText->getString().c_str(), portText->getString().c_str());

			client.Start();

			connectNode->removeFromParentAndCleanup(true);
		}
	});
	connectNode->addChild(connectButton);

	// Cancel UI button
	ui::Button *cancelButton = ui::Button::create(
		"Cancel.png");
	cancelButton->setPosition(Vec2(25.0f, winSize.height - 25.0f) + Director::getInstance()->getVisibleOrigin());
	cancelButton->setScale(0.25f);

	cancelButton->addTouchEventListener([connectNode](Ref* pSender, ui::Widget::TouchEventType type)
	{
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			connectNode->removeFromParentAndCleanup(true);
		}
	});
	connectNode->addChild(cancelButton);


	this->addChild(connectNode);

}

SnapshotClient::SnapshotClient()
{
}

SnapshotClient::~SnapshotClient()
{
}

void SnapshotClient::Init(const char * ip, const char * port)
{
}

bool SnapshotClient::Start()
{
	return false;
}

bool SnapshotClient::Stop()
{
	return false;
}

void SnapshotClient::Reset()
{
}

void SnapshotClient::SendPackets()
{
	switch (_state)
	{
	case(CLIENT_REQUESTING) :
	{
		//Packet* packet = CreatePacket(CLIENT_REQUESTING);
	}
	break;
	case(CLIENT_REQUEST_DENIED) :
	{

	}
	break;
	case(CLIENT_CONNECTED) :
	{

	}
	break;
	}
}

SnapshotServer::SnapshotServer()
{
}

SnapshotServer::~SnapshotServer()
{
}

void SnapshotServer::Init(char * port)
{
}

bool SnapshotServer::Start()
{
	return false;
}

bool SnapshotServer::Stop()
{
	return false;
}

void SnapshotServer::Reset()
{
}

void SnapshotServer::SendPackets()
{
	udp::resolver::query query(udp::v4(), "localhost", "1500");
	asio::ip::address::from_string("localhost");

}
