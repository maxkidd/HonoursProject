#include "SnapshotInterpolationLayer.h"

#include <thread>
#include <utility>

#include "cocos\ui\UITextField.h"
#include "cocos\ui\UIButton.h"
#include "cocos\ui\UIWidget.h"

#include <chrono>

using namespace std;
using namespace cocos2d;

SnapshotInterpolationLayer::SnapshotInterpolationLayer() : _statusLabel(nullptr), client()
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
	
	Menu* menu3 = Menu::create(connectItem, NULL);
	menu3->alignItemsHorizontally();
	menu3->setScale(1 / Director::getInstance()->getContentScaleFactor());
	menu3->setPosition(Vec2(winSize.width / 2.0f, winSize.height - 10.0f) + Director::getInstance()->getVisibleOrigin());

	addChild(menu3);

	createNetworkStatsLabel();

	//std::thread bt(std::bind(&SnapshotInterpolationLayer::setupNetwork, this));
	//bt.detach();

	setupServer();

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

		if (server.IsActive()) // Server
		{
			server.SendPackets();

			//server.WritePackets();
			//server.ReadPackets();

			server.ReceivePackets();

			debugString.append("Server: ");
		}
		else if (client.IsActive()) // Client
		{
			client.SendPackets();

			client.WritePackets();
			client.ReadPackets();

			client.ReceivePackets();

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

void SnapshotInterpolationLayer::setupServer()
{
	server.Init();

	server.Start();
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
				server.Stop();
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

SnapshotClient::SnapshotClient() : 
	_state(CLIENT_SLEEP), _transport(new SocketTransport(&_packetFactory))
{
}


SnapshotClient::~SnapshotClient()
{
	_state = CLIENT_SLEEP;
}

void SnapshotClient::Init(const char * ip, const char * port)
{
	_serverIP = ip;
	_serverPort = port;

	udp::resolver resolver{ *_transport->GetIOService() };
	_serverEndpoint = *resolver.resolve({ udp::v4(), ip, port });

}

bool SnapshotClient::Start()
{
	// Update state
	_state = CLIENT_REQUESTING;
	_active = true;


	return true;
}

bool SnapshotClient::Stop()
{
	_state = CLIENT_SLEEP;
	_active = false;
	return true;
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
		ConnectionRequestPacket* packet = (ConnectionRequestPacket*)CreateRequestPacket();
		_transport->SendPacket(_serverEndpoint, packet);
	}
	break;
	case(CLIENT_REQUEST_DENIED) :
	{
		_error = CLIENT_ERROR_REQUEST_DENIED; // TODO: received request
		Stop();
	}
	break;
	case(CLIENT_CONNECTED) :
	{
		Packet* packet = CreateConnectionPacket();
		_transport->SendPacket(_serverEndpoint, packet);
	}
	break;
	default:
	break;
	}
}

void SnapshotClient::ReceivePackets()
{
}

void SnapshotClient::WritePackets()
{
	_transport->WritePackets();
}

void SnapshotClient::ReadPackets()
{
	_transport->ReadPackets();
}

Packet* SnapshotClient::CreateRequestPacket()
{
	ConnectionRequestPacket* packet = new ConnectionRequestPacket();
	

	//packet->data;

	return packet;
}

Packet * SnapshotClient::CreateConnectionPacket()
{
	Packet* packet  = (Packet*)_connection->GeneratePacket();


	return packet;
}

SnapshotServer::SnapshotServer() : _transport(new SocketTransport(&_packetFactory))
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
	_active = true;

	return true;
}

bool SnapshotServer::Stop()
{
	_active = false;

	return true;
}

void SnapshotServer::Reset()
{
}

void SnapshotServer::SendPackets()
{
	//udp::resolver::query query(udp::v4(), "localhost", "1500");
	//asio::ip::address::from_string("localhost");
}

void SnapshotServer::ReceivePackets()
{
	while (true)
	{
		Packet* packet = _transport->ReceivePacket();
		if (!packet)
			break;

		int type = packet->GetType();

		
	}

}

void SnapshotServer::WritePackets()
{
	_transport->WritePackets();
}

void SnapshotServer::ReadPackets()
{
	_transport->ReadPackets();
}
