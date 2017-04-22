#include "MainMenu.h"

/// Layers
#include "Box2DTestBed/Box2DTest.h"
#include "SnapshotInterpolationLayer.h"
#include "StateSyncLayer.h"

//USING_NS_CC;
using namespace cocos2d;

Scene* MainMenu::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainMenu::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainMenu::init()
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
                                           CC_CALLBACK_1(MainMenu::menuCloseCallback, this));
    
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
    
    auto label = Label::createWithTTF("Synchronizing Physics over a Network", "fonts/Marker Felt.ttf", 30);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height - 10.0f));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto cocosSprite = Sprite::create("HelloWorld.png");
	cocosSprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 150.0f));
	this->addChild(cocosSprite, 0);

	// Menu
	MenuItemFont::setFontSize(25);
	MenuItemFont::setFontName("fonts/UbuntuMono-R.ttf");

	//auto clientItem = MenuItemFont::create("Client", CC_CALLBACK_0(HelloWorld::LoadClient, this));
	//auto serverItem = MenuItemFont::create("Server", CC_CALLBACK_0(HelloWorld::LoadServer, this));
	//auto networkItem = MenuItemFont::create("Network", CC_CALLBACK_0(HelloWorld::LoadNetwork, this));
	//auto box2DItem = MenuItemFont::create("Box2D", CC_CALLBACK_0(MainMenu::LoadBox2DTest, this));
	auto snapshotItem = MenuItemFont::create("Snapshot Interpolation", CC_CALLBACK_0(MainMenu::LoadSnapshot, this));
	auto stateSyncItem = MenuItemFont::create("State Synchronization", CC_CALLBACK_0(MainMenu::LoadStateSync, this));


	Menu* menu2 = Menu::create(snapshotItem, stateSyncItem, NULL);
	menu2->alignItemsHorizontally();
	addChild(menu2);

	//ImGUI
	

    return true;
}


void MainMenu::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void MainMenu::LoadBox2DTest()
{
	Director::getInstance()->replaceScene(Box2DTest::create());
}

void MainMenu::LoadSnapshot()
{
	Director::getInstance()->replaceScene(TransitionSplitCols::create(1.0f, SnapshotInterpolationLayer::scene()));
}

void MainMenu::LoadStateSync()
{
	Director::getInstance()->replaceScene(TransitionSplitCols::create(1.0f, StateSyncLayer::scene()));
}
