#include "SnapshotInterpolationLayer.h"

#include <thread>
#include <utility>

#include "cocos\ui\UITextField.h"
#include "cocos\ui\UIButton.h"
#include "cocos\ui\UIWidget.h"

#include <chrono>

using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;

SnapshotInterpolationLayer::SnapshotInterpolationLayer() : _statusLabel(nullptr),
server(nullptr), client(nullptr)
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


	auto connectItem = MenuItemFont::create("Connect to Server", CC_CALLBACK_0(SnapshotInterpolationLayer::connectAsClient, this));
	auto serverItem = MenuItemFont::create("Create Server", CC_CALLBACK_0(SnapshotInterpolationLayer::connectAsServer, this));
	
	Menu* menu3 = Menu::create(connectItem, serverItem, NULL);
	menu3->alignItemsHorizontally();
	menu3->setScale(1 / Director::getInstance()->getContentScaleFactor());
	menu3->setPosition(Vec2(winSize.width / 2.0f, winSize.height - 10.0f) + Director::getInstance()->getVisibleOrigin());

	addChild(menu3);

	createNetworkStatsLabel();

	_tableView = TableView::create(&_netDebugData, Size(winSize.width, winSize.height * 0.9f));
	_tableView->setPosition(10.0f, winSize.height * 0.1f);
	//tableView->setContentSize(Size(200.0f, 20.0f));
	_tableView->setDirection(ScrollView::Direction::VERTICAL);
	
	addChild(_tableView);
	_tableView->reloadData();

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

	_networkTimer += dt;

	while (_networkTimer > 1.0f) // Client or Server tick
	{
		_networkTimer -= (1.0f);

		if (server && server->IsActive()) // Server
		{
			server->SendPackets();

			server->WritePackets();
			server->ReadPackets();

			server->ReceivePackets();

			debugString.append("Server: " + server->GetNetworkState());
		}
		else if (client && client->IsActive()) // Client
		{
			client->SendPackets();

			client->WritePackets();
			client->ReadPackets();

			client->ReceivePackets();

			debugString.append("Client: " + client->GetNetworkState());
		}
		else
		{
			debugString.append("No Client/Server running");
		}
		_tableView->reloadData();

		_statusLabel->setString(debugString);
	}

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

void SnapshotInterpolationLayer::setupServer()
{
}

void SnapshotInterpolationLayer::connectAsClient()
{
	Size winSize = Director::getInstance()->getWinSize();

	Node *connectNode = Node::create();

	// IP address field
	ui::TextField *ipText = ui::TextField::create("IP ADDRESS", "Ariel", 50);
	ipText->setString("localhost");
	ipText->setMaxLength(12);
	ipText->setMaxLengthEnabled(true);
	ipText->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f  + 30.0f) + Director::getInstance()->getVisibleOrigin());
	connectNode->addChild(ipText);

	// Port address field
	ui::TextField *portText = ui::TextField::create("PORT", "Ariel", 50);
	portText->setString("1500");
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
			if (server)
			{
				if(server->IsActive())
					server->Stop();

				//server->destroy
				delete server;
				server = nullptr;
			}
			if(!client)
				client = new SnapshotClient(&_netDebugData);

			client->Init(ipText->getString().c_str(), portText->getString().c_str());

			client->Start();

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

void SnapshotInterpolationLayer::connectAsServer()
{

	if (client)
	{
		if (client->IsActive())
			client->Stop();

		//server->destroy
		delete client;
		client = nullptr;
	}
	if(!server)
		server = new SnapshotServer(&_netDebugData);

	server->Start();
}
