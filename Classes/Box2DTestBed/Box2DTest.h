#ifndef _BOX2DTEST_H_
#define _BOX2DTEST_H_

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "Test.h"
#include "WorldState.h"

//typedef Test* TestCreateFcn();
//struct TestEntry
//{
//	const char *name;
//	TestCreateFcn *createFcn;
//};

extern TestEntry g_testEntries[];
extern int g_totalEntries;

class Box2DTest : public cocos2d::Scene
{
public:
	CREATE_FUNC(Box2DTest);

	Box2DTest();
	~Box2DTest();

	bool initWithEntryID(int entryId);
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
	{
		return true;
	}

	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
	{
		auto diff = touch->getDelta();
		auto node = getChildByTag(0);
		auto currentPos = node->getPosition();
		node->setPosition(currentPos + diff);
	}
protected:
	void addTestCase(const std::string& testName, std::function<cocos2d::Layer*()> callback)
	{
		if (!testName.empty() && callback)
		{
			_childTestNames.emplace_back(testName);
			_testCallbacks.emplace_back(callback);
		}
	}

	std::vector<std::string> _childTestNames;
	std::vector<std::function<cocos2d::Layer*()>> _testCallbacks;

	int  m_entryID;
	cocos2d::EventListenerTouchOneByOne* _touchListener;
	Box2DView* m_view = nullptr;
};

class Box2DView : public cocos2d::Layer
{
	cocos2d::EventListenerTouchOneByOne* _touchListener;
	cocos2d::EventListenerKeyboard* _keyboardListener;
	TestEntry*    m_entry;
	Test*        m_test;
	int            m_entryID;
	WorldState* m_worldState = nullptr;
public:
	Box2DView(void);
	virtual ~Box2DView(void);

	bool initWithEntryID(int entryId);
	std::string title() const;
	virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;

	// World Functions
	void SaveWorld();
	void LoadWorld();

	//    virtual void registerWithTouchDispatcher();
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)override;
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)override;
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)override;

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event)override;
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event)override;
	//virtual void accelerometer(UIAccelerometer* accelerometer, cocos2d::Acceleration* acceleration);

	static Box2DView* viewWithEntryID(int entryId);
protected:
	void onDraw(const cocos2d::Mat4& transform, uint32_t flags);

	cocos2d::CustomCommand _customCmd;
};

#endif