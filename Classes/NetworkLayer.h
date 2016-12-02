#ifndef _NETWORKLAYER_H_
#define _NETWORKLAYER_H_


#include "cocos2d.h"
//#include "extensions/cocos-ext.h"
#include "ui/CocosGUI.h"
//#include "VisibleRect.h"
#include "NetworkDebugView.h"

class NetworkLayer : public cocos2d::Scene
{
public:
	CREATE_FUNC(NetworkLayer);

	NetworkLayer();
	~NetworkLayer();

	virtual bool init() override;
	//virtual void update(float dt);

	virtual cocos2d::ui::ScrollView::Direction getListViewDirection() const
	{
		return cocos2d::ui::ScrollView::Direction::VERTICAL;
	}
protected:
	cocos2d::Layer* _uiLayer;
	//cocos2d::ui::Layout* _widget;
	//cocos2d::ui::Text* _sceneTitle;

	NetworkDebugView* _networkDebug;
	cocos2d::ui::Text* _titleLabel;
	cocos2d::ui::Text* _indexLabels[5];

	float timer = 0.0f;
};

#endif