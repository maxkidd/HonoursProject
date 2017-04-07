#include "StateSyncLayer.h"

#include <thread>
#include <utility>

#include "cocos\ui\UITextField.h"
#include "cocos\ui\UIButton.h"
#include "cocos\ui\UIWidget.h"

#include <chrono>

#include "network_common.h"

using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;

StateSyncLayer::StateSyncLayer() : _statusLabel(nullptr)
//server(nullptr), client(nullptr)
	,_transport(new UnreliablePacketFactory(), new StateSyncMessageFactory())
{
	_simulation = C_StateSyncSimulation::create();
	addChild(_simulation);
}

StateSyncLayer::~StateSyncLayer()
{
	_eventDispatcher->removeEventListener(_touchListener);

	unscheduleUpdate();

	if (_statusLabel)
	{
		_statusLabel = nullptr;
	}
	if (client)
	{
		client->Disconnect();
		delete client;
		client = nullptr;
	}

	if (server)
		delete server;

	if (_simulation)
		delete _simulation;


	if (_tableView)
	{
		//_tableView->release();
		delete _tableView;
		_tableView = nullptr;
	}
}

cocos2d::Scene * StateSyncLayer::scene()
{
	auto scene = Scene::create();
	StateSyncLayer* layer = StateSyncLayer::create();
	scene->addChild(layer);

	return scene;
}

bool StateSyncLayer::init()
{
	Layer::init();

	// Listeners
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(false);

	listener->onTouchBegan = CC_CALLBACK_2(StateSyncLayer::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(StateSyncLayer::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(StateSyncLayer::onTouchEnded, this);

	_eventDispatcher->addEventListenerWithFixedPriority(listener, -10);
	_touchListener = listener;


	Size winSize = Director::getInstance()->getWinSize();

	// Menu
	MenuItemFont::setFontSize(16);
	MenuItemFont::setFontName("fonts/UbuntuMono-R.ttf");


	auto connectItem = MenuItemFont::create("Connect to Server", CC_CALLBACK_0(StateSyncLayer::connectAsClient, this));
	auto serverItem = MenuItemFont::create("Create Server", CC_CALLBACK_0(StateSyncLayer::connectAsServer, this));
	
	Menu* menu3 = Menu::create(connectItem, serverItem, NULL);
	menu3->alignItemsHorizontally();
	menu3->setScale(1 / Director::getInstance()->getContentScaleFactor());
	menu3->setPosition(Vec2(winSize.width / 2.0f, winSize.height - 30.0f) + Director::getInstance()->getVisibleOrigin());

	addChild(menu3);

	createNetworkStatsLabel();


	_tableView = TableView::create(_transport.GetDebugService(), Size(winSize.width, winSize.height * 0.9f));
	_tableView->setPosition(10.0f, winSize.height * 0.1f);
	//tableView->setContentSize(Size(200.0f, 20.0f));
	_tableView->setDirection(ScrollView::Direction::VERTICAL);
	_tableView->setTouchEnabled(false);
	//_tableView->retain();
	

	addChild(_tableView);
	//_tableView->reloadData();

	scheduleUpdate();

	return true;
}

void StateSyncLayer::ReloadScene()
{
	Director::getInstance()->replaceScene(StateSyncLayer::scene());
}

void StateSyncLayer::update(float dt)
{
	std::string debugString;

	_networkTimer += dt;

	static float updateTimer = 0.0f;
	static float inputTimer = 0.0f;
	updateTimer += dt;
	inputTimer += dt;


	_networkTimer -= (0.1f);

	if (server && server->IsActive()) // Server
	{
		server->ProcessMessages();
		while (updateTimer > 1.0f / 20.0f)
		{
			server->GenerateMessages();
			server->SendPackets();
			server->WritePackets();

			updateTimer -= 1.0f / 20.0f;
		}

		server->ReadPackets();
		server->ReceivePackets();

		debugString.append("Server: " + server->GetNetworkState()
			+ " Port: " + std::to_string(_transport.GetPort()));

		_tableView->reloadData();
	}
	else if (client && client->IsActive()) // Client
	{
		client->ProcessMessages();
		while (inputTimer > (1.0f / 60.0f))
		{
			client->GenerateMessages();
			client->SendPackets();
			client->WritePackets();
			inputTimer -= 1.0f / 60.0f;
		}

		client->ReadPackets();
		client->ReceivePackets();

		debugString.append("Client: " + client->GetNetworkState() 
			+ " Port: " + std::to_string(_transport.GetPort()));

		_tableView->reloadData();
	}
	else
	{
		debugString.append("No Client/Server running");
	}

	_statusLabel->setString(debugString);



	_physicsTimer += dt;
	while (_physicsTimer > 1.0f / 60.0f)
	{
		_simulation->Step();

		_physicsTimer -= (1.0f / 60.0f);
	}

}

bool StateSyncLayer::onTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event)
{
	auto pos = convertToNodeSpace(touch->getLocation());
	return _simulation->MouseDown(b2Vec2(pos.x / 15.0f, pos.y / 15.0f));;
}

void StateSyncLayer::onTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event)
{
	auto pos = convertToNodeSpace(touch->getLocation());
	return _simulation->MouseMove(b2Vec2(pos.x / 15.0f, pos.y / 15.0f));;
}

void StateSyncLayer::onTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event)
{
	auto pos = convertToNodeSpace(touch->getLocation());
	return _simulation->MouseUp(b2Vec2(pos.x / 15.0f, pos.y / 15.0f));;
}

void StateSyncLayer::createNetworkStatsLabel()
{
	Size winSize = Director::getInstance()->getWinSize();

	_statusLabel = LabelTTF::create("Not Connected", "Ariel", 12, winSize, TextHAlignment::RIGHT, TextVAlignment::BOTTOM);

	addChild(_statusLabel);

	_statusLabel->setScale(1 / Director::getInstance()->getContentScaleFactor());

	const int height_spacing = 22 / Director::getInstance()->getContentScaleFactor();
	_statusLabel->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f + height_spacing * 0) + Director::getInstance()->getVisibleOrigin());
}

void StateSyncLayer::setupServer()
{
}

void StateSyncLayer::connectAsClient()
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
				//if(server->IsActive())
				//	server->Stop();

				//server->destroy
				delete server;
				server = nullptr;
			}


			if (client)
			{
				delete client;
			}

			if (_simulation)
				removeChild(_simulation);
			_simulation = C_StateSyncSimulation::create();
			addChild(_simulation, 9999);

			client = new StateSyncClient((C_StateSyncSimulation*)_simulation, &_transport);
			client->Connect(ipText->getString().c_str(), portText->getString().c_str());

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

void StateSyncLayer::connectAsServer()
{

	if (client)
	{
		//server->destroy
		delete client;
		client = nullptr;
	}
	if (server)
	{
		delete server;
	}
	if (_simulation)
		removeChild(_simulation);
	_simulation = S_StateSyncSimulation::create();
	addChild(_simulation, 9999);

	server = new StateSyncServer((S_StateSyncSimulation*)_simulation, &_transport);

}
