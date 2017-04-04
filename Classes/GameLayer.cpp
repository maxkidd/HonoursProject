#include "GameLayer.h"

#include "cocos\ui\UITextField.h"
#include "cocos\ui\UIButton.h"
#include "cocos\ui\UIWidget.h"

#include "HelloWorldScene.h" // Replace name

using namespace cocos2d;

GameLayer::GameLayer()
{
	Size winSize = Director::getInstance()->getWinSize();

	ui::Button *backButton = ui::Button::create(
		"BackButton.png");
	backButton->setPosition(Vec2(winSize.width - 25.0f, winSize.height - 25.0f) + Director::getInstance()->getVisibleOrigin());
	backButton->setScale(1.0f);

	backButton->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type)
	{
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			// Go back to menu
			Director::getInstance()->pushScene(TransitionFadeTR::create(1.0f, HelloWorld::createScene()));
		}
	});
	addChild(backButton);
}

GameLayer::~GameLayer()
{

}