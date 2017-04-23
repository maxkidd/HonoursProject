#include "AppDelegate.h"
#include "MainMenu.h"

#include "SnapshotInterpolationLayer.h"
#include "StateSyncLayer.h"


USING_NS_CC;

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	#include "ImGUI\IMGUIGLViewImpl.h"
	#include "ImGUI\ImGuiLayer.h"
	#include "ImGUI\imgui.h"
	#include "ImGUI\CCIMGUI.h"
#endif

static cocos2d::Size designResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = IMGUIGLViewImpl::createWithRect("Synchronizing Physics Simulation", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
        //glview = GLViewImpl::createWithRect("BoostTest", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("BoostTest");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60.0f);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    Size frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of medium size.
    if (frameSize.height > mediumResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > smallResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else
    {        
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    }

    register_all_packages();

    // create a scene. it's an autorelease object
    auto scene = MainMenu::createScene();

    // run
    director->runWithScene(scene);


	// ImGUI
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	director->getScheduler()->schedule([=](float dt)
	{
		Scene* scene = director->getRunningScene();
		if (scene->getChildByName("ImGUILayer") == nullptr)
		{
			scene->addChild(ImGuiLayer::create(), INT_MAX, "ImGUILayer");
		}
	}, this, 0, false, "checkImGUI");


	CCIMGUI::getInstance()->addImGUI([=]() {


		static bool testToggle = false;
		if (testToggle)
			ImGui::ShowTestWindow();
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("(State Sync menu)", NULL, false, false);
				if (ImGui::MenuItem("Debug menu", "")) {}
				if (ImGui::BeginMenu("Open..."))
				{

					if (ImGui::MenuItem("Snapshot Interpolation"))
					{
						//LoadSnapshot();
						//Director::getInstance()->
						//auto schedular = Director::getInstance()->getScheduler();
						//scheduleOnce(SEL_SCHEDULE(&LoadStateSyncScene), 0.0f);
						//runAction(Sequence::create()
						//Director::getInstance()->pushScene(TransitionSplitCols::create(1.0f, SnapshotInterpolationLayer::scene()));
						//Director::getInstance()->pushScene(MainMenu::createScene());
						Director::getInstance()->replaceScene(SnapshotInterpolationLayer::scene());
					}
					if (ImGui::MenuItem("State Synchronization"))
					{
						//Director::getInstance()->pushScene(MainMenu::createScene());
						Director::getInstance()->replaceScene(StateSyncLayer::scene());
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Connect"))
				{
					testToggle = true;
				}

				if (ImGui::MenuItem("Back"))
				{
					Director::getInstance()->replaceScene(MainMenu::createScene());
				}
				if (ImGui::MenuItem("Quit"))
				{
					Director::getInstance()->end();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Game"))
			{
				//if (ImGui::MenuItem("Quit", "Alt+F4")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Info"))
			{
				try {
					asio::io_service netService;
					udp::resolver   resolver(netService);
					udp::resolver::query query(udp::v4(), "google.com", "");
					udp::resolver::iterator endpoints = resolver.resolve(query);
					udp::endpoint ep = *endpoints;
					udp::socket socket(netService);
					socket.connect(ep);
					asio::ip::address addr = socket.local_endpoint().address();
					//std::cout << "My IP according to google is: " << addr.to_string() << std::endl;
					ImGui::MenuItem(addr.to_string().c_str(), NULL, false, false);
				}
				catch (std::exception& e) {

				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}, "MainMenu");

#endif
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
