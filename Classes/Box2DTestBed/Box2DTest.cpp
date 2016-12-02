#include "Box2DTest.h"


USING_NS_CC;

Settings settings;

extern int g_totalEntries;

Box2DTest::Box2DTest()
{
	for (int entryId = 0; entryId < g_totalEntries; ++entryId)
	{
		addTestCase(g_testEntries[entryId].name, [entryId]() {
			return Box2DView::viewWithEntryID(entryId);
		});
	}

	auto test = initWithEntryID(1);
	
}

Box2DTest::~Box2DTest()
{
	if (m_view)
	{
		delete m_view;
		m_view = nullptr;
	}
}

bool Box2DTest::initWithEntryID(int entryId)
{
	removeAllChildren();
	auto director = Director::getInstance();
	Vec2 visibleOrigin = director->getVisibleOrigin();
	Size visibleSize = director->getVisibleSize();

	if (entryId < 0)
		entryId = 0;
	else if (entryId >= g_totalEntries)
		entryId = g_totalEntries-1;

	m_entryID = entryId;

	m_view = Box2DView::viewWithEntryID(entryId);
	addChild(m_view, 0, 0);
	m_view->setScale(15);
	m_view->setAnchorPoint(Vec2(0, 0));
	m_view->setPosition(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height / 3);

	auto label = Label::createWithTTF(m_view->title().c_str(), "fonts/arial.ttf", 28);
	addChild(label, 1);
	label->setPosition(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height - 50);


	Size winSize = Director::getInstance()->getWinSize();

	// Back button
	auto backButton = ui::Button::create();

	backButton->setPosition(Vec2(50.0f, winSize.height - 20.0f) + Director::getInstance()->getVisibleOrigin());
	backButton->setTitleText("BACK");

	backButton->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
			initWithEntryID(--m_entryID);
			break;
		default:
			break;
		}
	});
	addChild(backButton);

	// Forward button
	auto forwardButton = ui::Button::create();

	forwardButton->setPosition(Vec2(winSize.width - 50.0f, winSize.height - 20.0f) + Director::getInstance()->getVisibleOrigin());
	forwardButton->setTitleText("FORWARD");

	forwardButton->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
			initWithEntryID(++m_entryID);
			break;
		default:
			break;
		}
	});
	addChild(forwardButton);

	// Save button
	auto saveButton = ui::Button::create();

	saveButton->setPosition(Vec2(200.0f, winSize.height - 20.0f) + Director::getInstance()->getVisibleOrigin());
	saveButton->setTitleText("SAVE");

	saveButton->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
			m_view->SaveWorld();
			break;
		default:
			break;
		}
	});
	addChild(saveButton);

	// Load button
	auto loadSaveButton = ui::Button::create();

	loadSaveButton->setPosition(Vec2(winSize.width - 200.0f, winSize.height - 20.0f) + Director::getInstance()->getVisibleOrigin());
	loadSaveButton->setTitleText("LOAD");

	loadSaveButton->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
			m_view->LoadWorld();
			break;
		default:
			break;
		}
	});
	addChild(loadSaveButton);

	// Adds touch event listener
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = CC_CALLBACK_2(Box2DTest::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(Box2DTest::onTouchMoved, this);

	_eventDispatcher->addEventListenerWithFixedPriority(listener, 1);

	_touchListener = listener;

	return true;
}


//------------------------------------------------------------------
//
// Box2DView
//
//------------------------------------------------------------------
Box2DView::Box2DView(void)
{
}

Box2DView* Box2DView::viewWithEntryID(int entryId)
{
	Box2DView* pView = new (std::nothrow) Box2DView();
	pView->initWithEntryID(entryId);
	pView->autorelease();

	return pView;
}

bool Box2DView::initWithEntryID(int entryId)
{
	m_entry = g_testEntries + entryId;
	m_test = m_entry->createFcn();


	// Adds Touch Event Listener
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = CC_CALLBACK_2(Box2DView::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(Box2DView::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(Box2DView::onTouchEnded, this);

	_eventDispatcher->addEventListenerWithFixedPriority(listener, -10);
	_touchListener = listener;

	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Box2DView::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Box2DView::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithFixedPriority(keyboardListener, -11);
	_keyboardListener = keyboardListener;

	return true;
}

std::string Box2DView::title() const
{
	return std::string(m_entry->name);
}

void Box2DView::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	Layer::draw(renderer, transform, flags);

	_customCmd.init(_globalZOrder, transform, flags);
	_customCmd.func = CC_CALLBACK_0(Box2DView::onDraw, this, transform, flags);
	renderer->addCommand(&_customCmd);
}

void Box2DView::onDraw(const Mat4 &transform, uint32_t flags)
{
	Director* director = Director::getInstance();
	CCASSERT(nullptr != director, "Director is null when seting matrix stack");
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

	GL::enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION);
	m_test->Step(&settings);
	m_test->m_world->DrawDebugData();
	CHECK_GL_ERROR_DEBUG();

	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

Box2DView::~Box2DView()
{
	// Removes Touch Event Listener
	_eventDispatcher->removeEventListener(_touchListener);
	_eventDispatcher->removeEventListener(_keyboardListener);
	delete m_test;

	if (m_worldState)
	{
		delete m_worldState;
		m_worldState = nullptr;
	}
}

void Box2DView::SaveWorld()
{
	if (m_worldState)
	{
		delete m_worldState;
		m_worldState = nullptr;
	}
	m_worldState = new WorldState(m_test->m_world);
}

void Box2DView::LoadWorld()
{
	if (m_worldState)
	{
		m_worldState->LoadState();
	}
}

bool Box2DView::onTouchBegan(Touch* touch, Event* event)
{
	auto touchLocation = touch->getLocation();

	auto nodePosition = convertToNodeSpace(touchLocation);
	log("Box2DView::onTouchBegan, pos: %f,%f -> %f,%f", touchLocation.x, touchLocation.y, nodePosition.x, nodePosition.y);

	return m_test->MouseDown(b2Vec2(nodePosition.x, nodePosition.y));
}

void Box2DView::onTouchMoved(Touch* touch, Event* event)
{
	auto touchLocation = touch->getLocation();
	auto nodePosition = convertToNodeSpace(touchLocation);

	log("Box2DView::onTouchMoved, pos: %f,%f -> %f,%f", touchLocation.x, touchLocation.y, nodePosition.x, nodePosition.y);

	m_test->MouseMove(b2Vec2(nodePosition.x, nodePosition.y));
}

void Box2DView::onTouchEnded(Touch* touch, Event* event)
{
	auto touchLocation = touch->getLocation();
	auto nodePosition = convertToNodeSpace(touchLocation);

	log("Box2DView::onTouchEnded, pos: %f,%f -> %f,%f", touchLocation.x, touchLocation.y, nodePosition.x, nodePosition.y);

	m_test->MouseUp(b2Vec2(nodePosition.x, nodePosition.y));
}

void Box2DView::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
	log("Box2dView:onKeyPressed, keycode: %d", code);
	m_test->Keyboard(static_cast<unsigned char>(code));
}

void Box2DView::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
	log("onKeyReleased, keycode: %d", code);
	m_test->KeyboardUp(static_cast<unsigned char>(code));
}
