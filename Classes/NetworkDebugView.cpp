#include "NetworkDebugView.h"

#include "ui\UIButton.h"
#include "2d\CCLabel.h"
#include "ui\UIText.h"
#include "cocos2d.h"

using namespace cocos2d;
using namespace cocos2d::ui;

NetworkDebugView::NetworkDebugView()
{
}

NetworkDebugView::~NetworkDebugView()
{
	unscheduleUpdate();
}

bool NetworkDebugView::init()
{
	if (!Widget::init())
	{
		return false;
	}
	// Change
	//Size layerSize = getContentSize();


	// Create the list view
	_debugList = ListView::create();
	_debugList->setDirection(cocos2d::ui::ScrollView::Direction::VERTICAL);
	_debugList->setBounceEnabled(true);
	_debugList->setBackGroundImage("cocosui/green_edit.png");
	_debugList->setBackGroundImageScale9Enabled(true);
	_debugList->setContentSize(Size(500.0f,500.0f));
	_debugList->setVisible(false);
	_debugList->setScrollBarPositionFromCorner(Vec2(7, 7));
	_debugList->setItemsMargin(2.0f);
	//_debugList->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_debugList->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
	_debugList->setPosition((Vec2(500.0f, 0.0f)));
	_debugList->setGravity(ListView::Gravity::RIGHT);
	_debugList->setMagneticType(ListView::MagneticType::BOTTOM);


	// Move list test
	auto move = MoveTo::create(1, (Vec2(500.0f, 0.0f)));
	auto move_bck = MoveTo::create(1, (Vec2(0.0f, 0.0f)));
	auto forever = RepeatForever::create(Sequence::create(move, move_bck, nullptr));
	//_debugList->runAction(forever);

	// Add list to children
	addChild(_debugList);

	// Button
	Button* viewButton = Button::create("cocosui/btn_exercise03_n.png", "cocosui/btn_exercise03_p.png");
	// open scale9 render
	//viewButton->ignoreContentAdaptWithSize(false);
	//viewButton->setTitleText("Hello scale9");
	viewButton->setPosition(Vec2(0.0f,0.0f));
	viewButton->setScale(0.2f);
	//viewButton->setContentSize(Size(40, 40));
	viewButton->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
	viewButton->setPressedActionEnabled(true);
	viewButton->addTouchEventListener(CC_CALLBACK_2(NetworkDebugView::viewButtonPress, this));
	addChild(viewButton);


	// Add list items
	static const Size BUTTON_SIZE(100, 30);
	for (int i = 0; i < 40; ++i)
	{
		auto pButton = Button::create("cocosui/button.png", "cocosui/buttonHighlighted.png");
		pButton->setContentSize(BUTTON_SIZE);
		pButton->setScale9Enabled(true);
		pButton->setTitleText(StringUtils::format("Button-%d", i));

		_debugList->pushBackCustomItem(pButton);
		pushBackDebugItem(NetworkDebugView::DebugType::DEFAULT, "Network Layer", "This is just a quick test");
	}

	// Schedule update
	scheduleUpdate();
	return true;
}

void NetworkDebugView::update(float dt)
{
	timer += dt;
	if (timer > 1.0f)
	{
		pushBackDebugItem(NetworkDebugView::DebugType::DEFAULT, "Network Layer", "This is just a quick test");

		timer -= 1.0f;
	}
}

void NetworkDebugView::pushBackDebugItem(DebugType type, std::string title, std::string text)
{
	auto label2 = Text::create(title.append(": ").append(text), "fonts/arial.ttf", 14);

	_debugList->pushBackCustomItem(label2);
	_debugList->jumpToBottom();
}

void NetworkDebugView::viewButtonPress(Ref * pSender, Widget::TouchEventType type)
{

	{
		switch (type)
		{
		case Widget::TouchEventType::BEGAN:
			//_displayValueLabel->setString(StringUtils::format("Touch Down"));
			break;

		case Widget::TouchEventType::MOVED:
			//_displayValueLabel->setString(StringUtils::format("Touch Move"));
			break;

		case Widget::TouchEventType::ENDED:
		{
			//_displayValueLabel->setString(StringUtils::format("Touch Up"));
			Button *btn = (Button*)pSender;

			stopActionByTag(VIEW_MOVE_TAG);
			stopActionByTag(VIEW_MOVE2_TAG);
			stopActionByTag(VIEW_VISIBLE_TAG);

			stopActionByTag(VIEW_LIST_TAG);
			stopActionByTag(VIEW_BTN_TAG);
			stopAllActions();
			if (_debugList->getNumberOfRunningActions() == 0)
			{
				if (_debugList->isVisible())
				{
					// Move and hide list
					auto move = MoveTo::create(1, (Vec2(500.0f, 0.0f)));
					move->setTag(VIEW_MOVE_TAG);
					auto hide = Hide::create();
					hide->setTag(VIEW_VISIBLE_TAG);
					auto listActions = Sequence::create(move, hide, nullptr);
					listActions->setTag(VIEW_LIST_TAG);
					_debugList->runAction(listActions);

					// Move btn
					auto move2 = MoveTo::create(1, (Vec2(0.0f, 0.0f)));
					move2->setTag(VIEW_MOVE2_TAG);
					auto btnActions = Sequence::create(move2, nullptr);
					btnActions->setTag(VIEW_BTN_TAG);
					btn->runAction(btnActions);

				}
				else
				{
					// Move and hide list
					auto move = MoveTo::create(1, (Vec2(0.0f, 0.0f)));
					move->setTag(VIEW_MOVE_TAG);
					auto show = Show::create();
					show->setTag(VIEW_VISIBLE_TAG);
					auto listActions = Sequence::create(show, move, nullptr);
					listActions->setTag(VIEW_LIST_TAG);
					_debugList->runAction(listActions);


					// Move btn
					auto move2 = MoveTo::create(1, (Vec2(-500.0f, 500.0f)));
					move2->setTag(VIEW_MOVE2_TAG);
					auto btnActions = Sequence::create(move2, nullptr);
					btnActions->setTag(VIEW_BTN_TAG);
					btn->runAction(btnActions);
				}
			}
				//btn->setScale9Enabled(true);
		}
		break;

		case Widget::TouchEventType::CANCELED:
			//_displayValueLabel->setString(StringUtils::format("Touch Cancelled"));
			break;

		default:
			break;
		}
	}
}
