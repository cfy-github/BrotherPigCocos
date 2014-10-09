//
//  InGameScene.cpp
//  BrotherPigCocos
//
//  Created by fychen on 10/7/14.
//
//

#include "InGameScene.h"

USING_NS_CC;

Scene* InGame::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = InGame::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

void InGame::menuRestartCallBack(Ref* pSender) {
    Director::getInstance()->replaceScene(InGame::createScene());
}

void InGame::touchGroundWolf(Node* node) {
    auto pSprite=(Sprite *)node;
    pSprite->setTexture(cacheWolfRun);
    wolfs[pSprite->getTag()].state=1;
}

void InGame::touchGroundBallon(Node* node) {
    auto pSprite=(Sprite *)node;
    //pSprite->setTexture(cacheBallon);
    int tag=pSprite->getTag();
    removeChildByTag(tag, true);
    pSprite->release();
    wolfs[tag-100].pBallon=nullptr;
}

void InGame::updateGame(float t) {
    /*if(numWolfs==MaxNumWolfs) return;
    auto wolfSize=cacheWolfLive->getContentSize();
    for(int i=0;i<MaxNumWolfs;++i) {
        if(wolfs[i].state==0) {//Live Wolf
            wolfs[i].y-=wolfs[i].dy;
            if(wolfs[i].y <= wolfSize.height * 0.5) { // Hit the ground
                removeChildByTag(wolfs[i].tag);
                wolfs[i].state=-1;
                wolfs[i].pSprite=nullptr;
                numWolfs--;
            }
        }
    }*/
    //TODO
}

void InGame::updateNewWolf(float t) {
    
    //Generate wolves
    
    auto randNum=random(0,4)%4;
    //CCLOG("%d\n", randNum);
    genWolf(randNum);
}

void InGame::GameOver() {
    //CCLOG("Game Over!");
    MenuItem* restartItem=MenuItemImage::create(
                                           "restart.png",
                                           "restartSel.png",
                                           CC_CALLBACK_1(InGame::menuRestartCallBack, this));
    MenuItem* closeItem=MenuItemImage::create(
                                        "CloseNormal.png",
                                         "CloseSelected.png",
                                         CC_CALLBACK_1(InGame::menuCloseCallback, this));
    auto visibaleSize=Director::getInstance()->getVisibleSize();
    
    auto pSprite=Sprite::create("game_over.png");
    pSprite->setPosition(visibaleSize.width*0.5, visibaleSize.height*0.5);
    
    auto spriteSize=pSprite->getContentSize();
    restartItem->setPosition(0.5 * restartItem->getContentSize().width, 0.5 * restartItem->getContentSize().height);
    closeItem->setPosition(spriteSize.width-0.5*closeItem->getContentSize().width, 0.5 * closeItem->getContentSize().height);
    
    cocos2d::Vector<MenuItem*> pMenuItems;
    
    pMenuItems.pushBack(restartItem);
    pMenuItems.pushBack(closeItem);
    
    auto menu=Menu::createWithArray(pMenuItems);
    menu->setPosition(Vec2::ZERO);
    pSprite->addChild(menu, 1);
    
    addChild(pSprite);
    
}

void InGame::attackWolf(Node* node) {
    auto ps=(Sprite *)node;
    //CCLOG("%d\n", ps->getTag());
    ps->setTexture(cacheWolfRun);
    ps->setFlippedX(true);
}

void InGame::holdWolf(Node *node) {
    auto ps=(Sprite *) node;
    ps->setFlippedX(false);
    ps->setTexture(cacheWolfLive);
}

void InGame::touchWall(Node *node) {
    //ToDo: change to hungary wolf
    auto pSprite=(Sprite*) node;
    if(numHungryWolfs>=MaxHungryWolvs) {
        GameOver();
        return;
    }
    numHungryWolfs++;
    int tag=pSprite->getTag();
    wolfs[tag].state=2;
    pSprite->setTexture(cacheWolfLive);
    auto visibleSize=Director::getInstance()->getVisibleSize();
    auto placeHungryWolf=Place::create(Vec2(visibleSize.width-0.15 * visibleSize.height, visibleSize.height * (0.15 + 0.2 * (numHungryWolfs-1))));
    pSprite->runAction(placeHungryWolf);
    
    auto delayHungryWolf=DelayTime::create(1.0f);
    auto toAttackWolf=CallFuncN::create(std::bind(&InGame::attackWolf, this, std::placeholders::_1));
    auto moveFoward=MoveBy::create(0.5f, Vec2(-0.1 * visibleSize.height, 0));
    auto toHoldWolf=CallFuncN::create(std::bind(&InGame::holdWolf, this, std::placeholders::_1));
    
    auto actSeq=Sequence::create(delayHungryWolf, toAttackWolf, moveFoward, moveFoward->reverse(), toHoldWolf, NULL);
    auto actRepeat=RepeatForever::create(actSeq);
    
    pSprite->runAction(actRepeat);
}

void InGame::genWolf(int k) {
    if(numWolfs==MaxNumWolfs) return;
    
    auto visibleSize=Director::getInstance()->getVisibleSize();
    float x=3.0f/32 * visibleSize.width * (2*k+1);
    float y=0.9 * visibleSize.height - 0.5 * cacheWolfLive->getContentSize().height;
    
    int i;
    for(i=0;i<MaxNumWolfs;++i) if(wolfs[i].state==-1) break;
    wolfs[i].x=x;
    wolfs[i].y=y;
    auto ps=Sprite::createWithTexture(cacheWolfLive);
    auto pb=Sprite::createWithTexture(cacheBallon);
    
    ps->retain();
    pb->retain();
    
    pb->setPosition(x, y);
    ps->setPosition(x, y-pb->getContentSize().height/2-ps->getContentSize().height/2);
    pb->setTag(i+100);
    ps->setTag(i);//Todo
    wolfs[i].pSprite=ps;
    wolfs[i].pBallon=pb;
    wolfs[i].tag=i;
    wolfs[i].state=0;
    numWolfs++;
    addChild(ps);
    addChild(pb);
    
    //Action sequence
    auto placeActionBallon=Place::create(Vec2(x, y));
    auto placeActionWolf=Place::create(Vec2(x, y-pb->getContentSize().height/2-ps->getContentSize().height/2));
    auto fallMoveBallon=MoveTo::create(9.0f, Vec2(x, ps->getContentSize().height+pb->getContentSize().height * 0.5));
    auto fallMoveWolf=MoveTo::create(9.0f, Vec2(x, ps->getContentSize().height * 0.5));
    auto touchGrndBallon=CallFuncN::create(std::bind(&InGame::touchGroundBallon, this, std::placeholders::_1));
    auto touchGrndWolf=CallFuncN::create(std::bind(&InGame::touchGroundWolf, this, std::placeholders::_1));
    auto runMove=MoveTo::create(4.0f-k * 0.5f, Vec2(visibleSize.width-visibleSize.height * 0.2-cacheWolfRun->getContentSize().width/2, cacheWolfRun->getContentSize().height/2));
    auto touchWal=CallFuncN::create(std::bind(&InGame::touchWall, this, std::placeholders::_1));
    auto fallSeqWolf=Sequence::create(placeActionWolf, fallMoveWolf, touchGrndWolf, runMove, touchWal, NULL);//Stop at touch Wall
    auto fallSeqBallon=Sequence::create(placeActionBallon, fallMoveBallon, touchGrndBallon, NULL);

    wolfs[i].pSprite->runAction(fallSeqWolf);
    wolfs[i].pBallon->runAction(fallSeqBallon);
}

bool InGame::cacheImages() {
    cachePigG=Director::getInstance()->getTextureCache()->addImage("pig_g.png");
    cacheWolfDead=Director::getInstance()->getTextureCache()->addImage("wolf_dead.png");
    cacheWolfLive=Director::getInstance()->getTextureCache()->addImage("wolf_live.png");
    cacheWolfRun=Director::getInstance()->getTextureCache()->addImage("run_wolf.png");
    cacheBallon=Director::getInstance()->getTextureCache()->addImage("Circle.png");
    if(cachePigG && cacheWolfDead && cacheWolfLive && cacheWolfRun) return true;
    else return false;
}

bool InGame::init() {
    
    if(!Layer::init()) {
        return false;
    }
    
    if(!cacheImages()) return false;
    
    this->schedule(schedule_selector(InGame::updateNewWolf), 4.0f, kRepeatForever, 2.0f);
    //this->schedule(schedule_selector(InGame::updateGame));
    
    //Init wolfInfo's
    numWolfs=0;
    numHungryWolfs=0;
    for(int i=0;i<10;++i) wolfs[i].state=-1;
    
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    //Draw Floor
    auto rectFloor = DrawNode::create();
    Vec2 vsFloor[] = {
        Vec2(0, visibleSize.height),
        Vec2(visibleSize.width * 0.75, visibleSize.height),
        Vec2(visibleSize.width * 0.75, visibleSize.height * 0.9),
        Vec2(0, visibleSize.height * 0.9)
    };
    rectFloor->drawPolygon(vsFloor, 4, Color4F(1.0f,0.3f,0.3f,1), 3, Color4F(0.2f,0.2f,0.2f,1));
    addChild(rectFloor, 0);
    
    //Block0
    auto rectBlock0 = DrawNode::create();
    Vec2 vsBlock0[] = {
        Vec2(visibleSize.width-visibleSize.height * 0.2, visibleSize.height * 0.1),
        Vec2(visibleSize.width, visibleSize.height * 0.1),
        Vec2(visibleSize.width, 0),
        Vec2(visibleSize.width-visibleSize.height * 0.2, 0)
    };
    rectBlock0->drawPolygon(vsBlock0, 4, Color4F(1.0f,0.3f,0.3f,1), 0, Color4F(1.0f,0.3f,0.3f,1));
    addChild(rectBlock0);
    
    //Block1
    auto rectBlock1 = DrawNode::create();
    Vec2 vsBlock1[] = {
        Vec2(visibleSize.width-visibleSize.height * 0.1, visibleSize.height * 0.2),
        Vec2(visibleSize.width, visibleSize.height * 0.2),
        Vec2(visibleSize.width, visibleSize.height * 0.1),
        Vec2(visibleSize.width-visibleSize.height * 0.1, visibleSize.height * 0.1)
    };
    rectBlock1->drawPolygon(vsBlock1, 4, Color4F(1.0f,0.3f,0.6f,1), 0, Color4F(1.0f,0.3f,0.6f,1));
    addChild(rectBlock1);
    
    //Block2
    auto rectBlock2 = DrawNode::create();
    Vec2 vsBlock2[] = {
        Vec2(visibleSize.width-visibleSize.height * 0.2, visibleSize.height * 0.3),
        Vec2(visibleSize.width, visibleSize.height * 0.3),
        Vec2(visibleSize.width, visibleSize.height * 0.2),
        Vec2(visibleSize.width-visibleSize.height * 0.2, visibleSize.height * 0.2)
    };
    rectBlock2->drawPolygon(vsBlock2, 4, Color4F(1.0f,0.3f,0.6f,1), 0, Color4F(1.0f,0.3f,0.6f,1));
    addChild(rectBlock2);
    
    //Block3
    auto rectBlock3 = DrawNode::create();
    Vec2 vsBlock3[] = {
        Vec2(visibleSize.width-visibleSize.height * 0.1, visibleSize.height * 0.4),
        Vec2(visibleSize.width, visibleSize.height * 0.4),
        Vec2(visibleSize.width, visibleSize.height * 0.3),
        Vec2(visibleSize.width-visibleSize.height * 0.1, visibleSize.height * 0.3)
    };
    rectBlock3->drawPolygon(vsBlock3, 4, Color4F(1.0f,0.3f,0.6f,1), 0, Color4F(1.0f,0.3f,0.6f,1));
    addChild(rectBlock3);
    
    //Block4
    auto rectBlock4 = DrawNode::create();
    Vec2 vsBlock4[] = {
        Vec2(visibleSize.width-visibleSize.height * 0.2, visibleSize.height * 0.5),
        Vec2(visibleSize.width, visibleSize.height * 0.5),
        Vec2(visibleSize.width, visibleSize.height * 0.4),
        Vec2(visibleSize.width-visibleSize.height * 0.2, visibleSize.height * 0.4)
    };
    rectBlock4->drawPolygon(vsBlock4, 4, Color4F(1.0f,0.3f,0.6f,1), 0, Color4F(1.0f,0.3f,0.6f,1));
    addChild(rectBlock4);
    
    //Block5
    auto rectBlock5 = DrawNode::create();
    Vec2 vsBlock5[] = {
        Vec2(visibleSize.width-visibleSize.height * 0.1, visibleSize.height * 0.6),
        Vec2(visibleSize.width, visibleSize.height * 0.6),
        Vec2(visibleSize.width, visibleSize.height * 0.5),
        Vec2(visibleSize.width-visibleSize.height * 0.1, visibleSize.height * 0.5)
    };
    rectBlock5->drawPolygon(vsBlock5, 4, Color4F(1.0f,0.3f,0.6f,1), 0, Color4F(1.0f,0.3f,0.6f,1));
    addChild(rectBlock5);
    
    //Block6
    auto rectBlock6 = DrawNode::create();
    Vec2 vsBlock6[] = {
        Vec2(visibleSize.width-visibleSize.height * 0.2, visibleSize.height * 0.7),
        Vec2(visibleSize.width, visibleSize.height * 0.7),
        Vec2(visibleSize.width, visibleSize.height * 0.6),
        Vec2(visibleSize.width-visibleSize.height * 0.2, visibleSize.height * 0.6)
    };
    rectBlock6->drawPolygon(vsBlock6, 4, Color4F(1.0f,0.3f,0.6f,1), 0, Color4F(1.0f,0.3f,0.6f,1));
    addChild(rectBlock6);
    
    //Block7
    auto rectBlock7 = DrawNode::create();
    Vec2 vsBlock7[] = {
        Vec2(visibleSize.width-visibleSize.height * 0.1, visibleSize.height * 0.8),
        Vec2(visibleSize.width, visibleSize.height * 0.8),
        Vec2(visibleSize.width, visibleSize.height * 0.7),
        Vec2(visibleSize.width-visibleSize.height * 0.1, visibleSize.height * 0.7)
    };
    rectBlock7->drawPolygon(vsBlock7, 4, Color4F(1.0f,0.3f,0.6f,1), 0, Color4F(1.0f,0.3f,0.6f,1));
    addChild(rectBlock7);
    
    //Block8
    auto rectBlock8 = DrawNode::create();
    Vec2 vsBlock8[] = {
        Vec2(visibleSize.width-visibleSize.height * 0.2, visibleSize.height * 0.85),
        Vec2(visibleSize.width, visibleSize.height * 0.85),
        Vec2(visibleSize.width, visibleSize.height * 0.8),
        Vec2(visibleSize.width-visibleSize.height * 0.2, visibleSize.height * 0.8)
    };
    rectBlock8->drawPolygon(vsBlock8, 4, Color4F(1.0f,0.3f,0.6f,1), 0, Color4F(1.0f,0.3f,0.6f,1));
    addChild(rectBlock8);
    
    //Block9
    auto rectBlock9 = DrawNode::create();
    Vec2 vsBlock9[] = {
        Vec2(visibleSize.width-visibleSize.height * 0.2, visibleSize.height * 0.9),
        Vec2(visibleSize.width, visibleSize.height * 0.9),
        Vec2(visibleSize.width, visibleSize.height * 0.85),
        Vec2(visibleSize.width-visibleSize.height * 0.2, visibleSize.height * 0.85)
    };
    rectBlock9->drawPolygon(vsBlock9, 4, Color4F(1.0f,0.8f,0.6f,1), 0, Color4F(1.0f,0.8f,0.6f,1));
    addChild(rectBlock9);
    
    return true;
}

void InGame::menuCloseCallback(Ref* pSender)
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

