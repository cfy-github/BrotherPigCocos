#include "HelloWorldScene.h"
#include "InGameScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    MenuItem* pCloseItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	pCloseItem->setPosition(Vec2(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));
    
    MenuItem* pStartItem = MenuItemImage::create(
                                           "PlayNow.png",
                                           "PlayNowSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuStartCallback, this));
    
    pStartItem->setPosition(Vec2(origin.x + pStartItem->getContentSize().width/2 ,
                                 origin.y + pStartItem->getContentSize().height/2));
    
    cocos2d::Vector<MenuItem*> pMenuItems;
    
    pMenuItems.pushBack(pStartItem);
    pMenuItems.pushBack(pCloseItem);

    // create menu, it's an autorelease object
    //auto menu = Menu::create(closeItem, NULL);
    
    auto menu=Menu::createWithArray(pMenuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = LabelTTF::create("Brother Pig", "Arial", 50);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("pig.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    
    sprite->setScale(visibleSize.width/sprite->getContentSize().width, visibleSize.height/sprite->getContentSize().height);

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
    return true;
}

void HelloWorld::menuStartCallback(Ref* pSender){
    //TODO
    CCLOG("START!\n");
    //CCLOG("%f <> %f\n", Director::getInstance()->getVisibleSize().width, Director::getInstance()->getVisibleSize().height);
    Director::getInstance()->replaceScene(InGame::createScene());
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
