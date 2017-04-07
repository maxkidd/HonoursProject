#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

//#define ASIO_STANDALONE 
//#define ASIO_HAS_STD_ADDRESSOF
//#define ASIO_HAS_STD_ARRAY
//#define ASIO_HAS_CSTDINT
//#define ASIO_HAS_STD_SHARED_PTR
//#define ASIO_HAS_STD_TYPE_TRAITS

#include <asio/asio.hpp>

class HelloWorld : public cocos2d::Layer
{
private:
	bool isShowDemo;
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	void LoadClient();
	void LoadServer();
	void LoadNetwork();
	void LoadBox2DTest();
	void LoadSnapshot();
	void LoadStateSync();
};

#endif // __HELLOWORLD_SCENE_H__
