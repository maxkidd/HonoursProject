#include "HelloWorldScene.h"

//#include "ClientLayer.h"
//#include "ServerLayer.h"
//#include "NetworkLayer.h"
#include "Box2DTestBed/Box2DTest.h"
#include "SnapshotInterpolationLayer.h"
#include "StateSyncLayer.h"

#include "ImGUI\CCIMGUI.h"

#include "asio\asio.hpp"

//USING_NS_CC;
using namespace cocos2d;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    

	// Menu
	MenuItemFont::setFontSize(16);
	MenuItemFont::setFontName("fonts/UbuntuMono-R.ttf");

	//auto clientItem = MenuItemFont::create("Client", CC_CALLBACK_0(HelloWorld::LoadClient, this));
	//auto serverItem = MenuItemFont::create("Server", CC_CALLBACK_0(HelloWorld::LoadServer, this));
	//auto networkItem = MenuItemFont::create("Network", CC_CALLBACK_0(HelloWorld::LoadNetwork, this));
	auto box2DItem = MenuItemFont::create("Box2D", CC_CALLBACK_0(HelloWorld::LoadBox2DTest, this));
	auto snapshotItem = MenuItemFont::create("Snapshot Interpolation", CC_CALLBACK_0(HelloWorld::LoadSnapshot, this));
	auto stateSyncItem = MenuItemFont::create("State Synchronization", CC_CALLBACK_0(HelloWorld::LoadStateSync, this));


	Menu* menu2 = Menu::create(box2DItem, snapshotItem, stateSyncItem, NULL);
	menu2->alignItemsVertically();
	addChild(menu2);


	//ImGUI
	
	CCIMGUI::getInstance()->addImGUI([=]() {
		//ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_Always);
		//ImGui::ShowTestWindow(&isShowDemo);

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("(State Sync menu)", NULL, false, false);
				if (ImGui::MenuItem("Debug menu", "")) {}
				if (ImGui::BeginMenu("Open..."))
				{

					if (ImGui::MenuItem("Snapshot Interpolation"))
					{
						Director::getInstance()->replaceScene(TransitionFadeBL::create(1.0f, SnapshotInterpolationLayer::scene()));
					}
					if (ImGui::MenuItem("State Synchronization"))
					{
						Director::getInstance()->replaceScene(TransitionSplitCols::create(1.0f, StateSyncLayer::scene()));
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Back", "F3")) 
				{
					Director::getInstance()->replaceScene(TransitionSplitCols::create(1.0f, HelloWorld::createScene()));;
				}
				if (ImGui::MenuItem("Quit", "F4")) 
				{
					Director::getInstance()->end();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Game"))
			{
				//if (ImGui::MenuItem("Quit", "Alt+F4")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Info"))
			{
				try {
					asio::io_service netService;
					udp::resolver   resolver(netService);
					udp::resolver::query query(udp::v4(), "google.com", "");
					udp::resolver::iterator endpoints = resolver.resolve(query);
					udp::endpoint ep = *endpoints;
					udp::socket socket(netService);
					socket.connect(ep);
					asio::ip::address addr = socket.local_endpoint().address();
					//std::cout << "My IP according to google is: " << addr.to_string() << std::endl;
					ImGui::MenuItem(addr.to_string().c_str(), NULL, false, false);
				}
				catch (std::exception& e) {

				}
				try {
					asio::io_service netService;
					udp::resolver   resolver(netService);
					udp::resolver::query query(udp::v4(), "uaddns02.uad.ac.uk", "");
					udp::resolver::iterator endpoints = resolver.resolve(query);
					udp::endpoint ep = *endpoints;
					udp::socket socket(netService);
					socket.connect(ep);
					asio::ip::address addr = socket.local_endpoint().address();
					//std::cout << "My IP according to google is: " << addr.to_string() << std::endl;
					ImGui::MenuItem(addr.to_string().c_str(), NULL, false, false);
				}
				catch (std::exception& e) {

				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}, "MainMenu");

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::LoadClient()
{
	//Director::getInstance()->replaceScene(ClientLayer::scene());
}

void HelloWorld::LoadServer()
{
	//Director::getInstance()->replaceScene(ServerLayer::scene());
}

void HelloWorld::LoadNetwork()
{
	//Director::getInstance()->replaceScene(NetworkLayer::create());
}

void HelloWorld::LoadBox2DTest()
{
	Director::getInstance()->replaceScene(Box2DTest::create());
}

void HelloWorld::LoadSnapshot()
{
	Director::getInstance()->replaceScene(TransitionFadeBL::create(1.0f, SnapshotInterpolationLayer::scene()));
}

void HelloWorld::LoadStateSync()
{
	Director::getInstance()->replaceScene(TransitionFadeBL::create(1.0f, StateSyncLayer::scene()));
}
