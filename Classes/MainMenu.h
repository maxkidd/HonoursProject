#ifndef _MAINMENU_H_
#define _MAINMENU_H_

#include "cocos2d.h"

#include <asio/asio.hpp>

class MainMenu : public cocos2d::Layer
{
private:
	bool isShowDemo;
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(MainMenu);

	void LoadBox2DTest();
	void LoadSnapshot();
	void LoadStateSync();
};

#endif // __HELLOWORLD_SCENE_H__
