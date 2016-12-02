#include "NetworkLayer.h"

USING_NS_CC;
using namespace cocos2d::ui;

NetworkLayer::NetworkLayer()
	: _uiLayer(nullptr)
{
}

NetworkLayer::~NetworkLayer()
{
}

bool NetworkLayer::init()
{
	if (Scene::init())
	{
		_uiLayer = Layer::create();
		addChild(_uiLayer);

		//_widget = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("cocosui/UITest/UITest.json"));
		//_uiLayer->addChild(_widget);

		Size screenSize = Director::getInstance()->getWinSize();
		//Size rootSize = _widget->getContentSize();
		Size rootSize = screenSize;// Size(0.0f, 0.0f);
		_uiLayer->setPosition(Vec2((screenSize.width - rootSize.width) / 2,
			(screenSize.height - rootSize.height) / 2));

		Layout* root = static_cast<Layout*>(_uiLayer->getChildByTag(81));


		Size layerSize = _uiLayer->getContentSize();

		_titleLabel = Text::create("Magnetic scroll", "fonts/Marker Felt.ttf", 32);
		_titleLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		_titleLabel->setPosition(Vec2(layerSize / 2) + Vec2(0, _titleLabel->getContentSize().height * 3.15f));
		_uiLayer->addChild(_titleLabel, 3);

		_networkDebug = NetworkDebugView::create();
		//_networkDebug->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		//_networkDebug->setContentSize(layerSize / 2.0f);
		_networkDebug->setPosition(Size(layerSize.width,0.0f));
		_uiLayer->addChild(_networkDebug);
		/*
		// Create the list view
		_networkDebug = NetworkDebugView::create();
		_networkDebug->setDirection(getListViewDirection());
		_networkDebug->setBounceEnabled(true);
		_networkDebug->setBackGroundImage("cocosui/green_edit.png");
		_networkDebug->setBackGroundImageScale9Enabled(true);
		_networkDebug->setContentSize(layerSize / 2);
		_networkDebug->setScrollBarPositionFromCorner(Vec2(7, 7));
		_networkDebug->setItemsMargin(2.0f);
		_networkDebug->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		_networkDebug->setPosition(layerSize / 2);
		_networkDebug->setGravity(ListView::Gravity::RIGHT);
		_networkDebug->setMagneticType(ListView::MagneticType::BOTTOM);
		_uiLayer->addChild(_networkDebug);


		// Initial magnetic type
		_titleLabel->setString("MagneticType - BOTTOM");

		// Add list items
		static const Size BUTTON_SIZE(100, 30);
		for (int i = 0; i < 40; ++i)
		{
			auto pButton = Button::create("cocosui/button.png", "cocosui/buttonHighlighted.png");
			pButton->setContentSize(BUTTON_SIZE);
			pButton->setScale9Enabled(true);
			pButton->setTitleText(StringUtils::format("Button-%d", i));
			_networkDebug->pushBackCustomItem(pButton);

			_networkDebug->pushBackDebugItem(NetworkDebugView::DebugType::DEFAULT, "Network Layer", "This is just a quick test");
				
		}

		scheduleUpdate();
		*/
		return true;
	}
	return false;
}
