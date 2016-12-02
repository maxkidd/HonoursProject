#ifndef _NETWORKDEBUGVIEW_H_
#define _NETWORKDEBUGVIEW_H_

//#include "cocos2d.h"

//#include "asio\asio.hpp"

//#include "cocos2d\cocos\ui\UIScrollView.h"
//#include "cocos2d\cocos\ui\UIText.h"
#include "cocos2d\cocos\ui\UIListView.h"

class NetworkDebugView : public cocos2d::ui::Widget
{
public:
	enum class DebugType
	{
		DEFAULT,
		NET_ERROR,
		NET_DEBUG,
	};

	NetworkDebugView();
	~NetworkDebugView();

	CREATE_FUNC(NetworkDebugView);
	virtual bool init() override;
	virtual void update(float dt);
	
    void pushBackDebugItem(DebugType type, std::string title, std::string text);


	void viewButtonPress(Ref *pSender, Widget::TouchEventType type);

protected:
	cocos2d::ui::ListView* _debugList = nullptr;

	float timer = 0.0f; // Timer for testing new items on update

	const unsigned int VIEW_MOVE_TAG = 42;
	const unsigned int VIEW_MOVE2_TAG = 43;
	const unsigned int VIEW_VISIBLE_TAG = 44;

	const unsigned int VIEW_LIST_TAG = 45;
	const unsigned int VIEW_BTN_TAG = 46;
};
#endif