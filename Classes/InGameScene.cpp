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

void InGame::removeWolf(Node* node) {
    auto ps=(Sprite *) node;
    int tag=ps->getTag();
    removeChildByTag(tag, true);
    ps->release();
    wolfs[tag].state=-1;
    wolfs[tag].pSprite=nullptr;
    numWolfs--;
}

void InGame::toDeadWolf(int t) {
    //Remove ballon sprite
    auto ballonTag=wolfs[t].pBallon->getTag();
    removeChildByTag(ballonTag, true);
    wolfs[t].pBallon->release();
    wolfs[t].pBallon=nullptr;
    
    //Change state of wolf to falling
    wolfs[t].state=-2; //-2 for falling;
    wolfs[t].pSprite->setTexture(cacheWolfDead);
    wolfs[t].pSprite->stopAllActions();
    auto toAction=MoveTo::create((wolfs[t].pSprite->getPosition().y-wolfs[t].pSprite->getContentSize().height*0.5)/300.0f, Vec2(wolfs[t].pSprite->getPosition().x, wolfs[t].pSprite->getContentSize().height*0.5));
    auto actRemoveWolf=CallFuncN::create(std::bind(&InGame::removeWolf, this, std::placeholders::_1));
    auto actSeq=Sequence::create(toAction, actRemoveWolf, NULL);//???
    wolfs[t].pSprite->runAction(actSeq);
}

void InGame::updateGame(float t) {
    auto ps=getChildByTag(610);
    auto from=ps->getPosition();
    auto visibleSize=Director::getInstance()->getVisibleSize();
    
    if(getMovingDown()) {
        if(from.y>=3) {
            ps->setPosition(Vec2(from.x, from.y-3)); //Dy=3
        }
    }
    
    if(getMovingUp()) {
        if(from.y<=visibleSize.height * 0.8 - 3) {
            ps->setPosition(Vec2(from.x, from.y+3));
        }
    }
    
    //Check if the pig touches brick
    //int pigTag=610;
    //int brickTag=700;
    auto pPig=getChildByTag(610);
    auto pBrick=getChildByTag(700);
    
    if(pBrick && pPig->getBoundingBox().intersectsRect(pBrick->getBoundingBox())) {
        auto pigPos=pPig->getPosition();
        pBrick->setPosition(pigPos.x-pBrick->getContentSize().width, pigPos.y);
        holdBrick=true;
    }
    
    //Update dropping brick
    if(pBrick && brickFired) {
        //Hitting check
        for(int i=0;i<MaxNumWolfs;++i) {
            if(wolfs[i].state==0) {
                if(pBrick->getBoundingBox().intersectsRect(wolfs[i].pBallon->getBoundingBox()) ||
                   pBrick->getBoundingBox().intersectsRect(wolfs[i].pSprite->getBoundingBox())) {
                    //Hit wolf
                    toDeadWolf(i);
                }
            }
        }
        //Remove brick if it goes out of range
        if(pBrick->getPosition().x<=0 || pBrick->getPosition().y<=0) {
            removeChildByTag(700);
            numBricks--;
            brickFired=false;
            brickDy=0;
        } else {
            brickDy+=0.085;
            pBrick->setPosition(pBrick->getPosition().x-9, pBrick->getPosition().y-brickDy);
        }
    }
    
    //falling wolf hits living wolf
    for(int i=0;i<MaxNumWolfs;++i) {
        if(wolfs[i].state==-2) {
            for(int j=0;j<MaxNumWolfs;++j) {
                if(wolfs[j].state==0) {
                    if(wolfs[i].pSprite->getBoundingBox().intersectsRect(wolfs[j].pSprite->getBoundingBox()) ||
                       wolfs[i].pSprite->getBoundingBox().intersectsRect(wolfs[j].pBallon->getBoundingBox())) {
                        toDeadWolf(j);
                    }
                }
            }
        }
    }
}

void InGame::updateNewWolf(float t) {
    
    //Generate wolves
    
    auto randNum=random(0,4)%4;
    //CCLOG("%d\n", randNum);
    genWolf(randNum);
}

void InGame::GameOver() {
    MenuItem* restartItem=MenuItemImage::create(
                                           "restart.png",
                                           "restartSel.png",
                                           CC_CALLBACK_1(InGame::menuRestartCallBack, this));
    MenuItem* closeItem=MenuItemImage::create(
                                        "CloseNormal.png",
                                         "CloseSelected.png",
                                         CC_CALLBACK_1(InGame::menuCloseCallback, this));
    auto visibaleSize=Director::getInstance()->getVisibleSize();
    
    restartItem->setScale(0.6);
    restartItem->setOpacity(128);
    
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

void InGame::menuFireCallBack(Ref* pSender){
    //CCLOG("Fire!\n");
    if(holdBrick) {
        brickFired=true;
        holdBrick=false;
    }
}

void InGame::touchWall(Node *node) {
    //ToDo: change to hungary wolf
    auto pSprite=(Sprite*) node;
    if(numHungryWolfs>=MaxHungryWolvs) {
        this->pause();
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

void InGame::initTouchEvent() {
    auto listener=EventListenerTouchOneByOne::create();
    
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan=CC_CALLBACK_2(InGame::onTouchBegan, this);
    listener->onTouchMoved=CC_CALLBACK_2(InGame::onTouchMoved, this);
    listener->onTouchEnded=CC_CALLBACK_2(InGame::onTouchEnded, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void InGame::setMovingDown(bool flag) {
    movingDown=flag;
}

bool InGame::getMovingDown() {return movingDown;}

void InGame::setMovingUp(bool flag) {
    movingUp=flag;
}

bool InGame::getMovingUp() {return movingUp;}



void InGame::onTouchEnded(Touch* t, Event* e) {
    CCLOG("Touch ended!\n");
    auto visibleSize=Director::getInstance()->getVisibleSize();
    
    auto tLoc=t->getLocationInView();
    tLoc=Director::getInstance()->convertToGL(tLoc);
    
    auto psUp=getChildByTag(600); //600 for up button;
    auto psDown=getChildByTag(601); //601 for down button;
    
    if(psUp->getBoundingBox().containsPoint(Point(tLoc.x-visibleSize.width+visibleSize.height*0.1,tLoc.y-visibleSize.height*0.1))) {
        setMovingUp(false);
        psUp->setColor(Color3B(Color4F(0.1f,0.5f,0.6f,1)));
    }
    
    if(psDown->getBoundingBox().containsPoint(Point(tLoc.x-visibleSize.width+visibleSize.height*0.1,tLoc.y))) {
        setMovingDown(false);
        psDown->setColor(Color3B(Color4F(0.1f,0.5f,0.6f,1)));
    }
}

bool InGame::onTouchBegan(Touch* t, Event* e) {
    CCLOG("Touch Began\n");
    auto visibleSize=Director::getInstance()->getVisibleSize();
    auto tLoc=t->getLocationInView();
    tLoc=Director::getInstance()->convertToGL(tLoc);
    
    auto psUp=getChildByTag(600); //600 for up button;
    auto psDown=getChildByTag(601); //601 for down button;
    
    auto bbox=psDown->getBoundingBox();
    
    CCLOG("%f,%f,%f,%f\n", bbox.origin.x, bbox.origin.y, bbox.size.width, bbox.size.height);
    CCLOG("%f, %f\n", tLoc.x, tLoc.y);

    
    if(psUp->getBoundingBox().containsPoint(Point(tLoc.x-visibleSize.width+visibleSize.height*0.1,tLoc.y-visibleSize.height*0.1))) {
        setMovingUp(true);
        setMovingDown(false);
        psUp->setColor(Color3B(Color4F(0.6f,0.5f,0.1f,1)));
        CCLOG("up btn\n");
    }
    
    if(psDown->getBoundingBox().containsPoint(Point(tLoc.x-visibleSize.width+visibleSize.height*0.1,tLoc.y))) {
        setMovingDown(true);
        setMovingUp(false);
        psDown->setColor(Color3B(Color4F(0.6f,0.5f,0.1f,1)));
        CCLOG("DOWN BTN\n");
    }
    
    return true;
}

void InGame::onTouchMoved(Touch* t, Event* e) {
    auto tLoc=t->getLocationInView();
    
    auto visibleSize=Director::getInstance()->getVisibleSize();
    tLoc=Director::getInstance()->convertToGL(tLoc);
    
    auto psUp=getChildByTag(600); //600 for up button;
    auto psDown=getChildByTag(601); //601 for down button;
    
    if(psUp->getBoundingBox().containsPoint(Point(tLoc.x-visibleSize.width+visibleSize.height*0.1,tLoc.y-visibleSize.height*0.1))) {
        setMovingUp(true);
        setMovingDown(false);
    }
    
    if(psDown->getBoundingBox().containsPoint(Point(tLoc.x-visibleSize.width+visibleSize.height*0.1,tLoc.y))) {
        setMovingDown(true);
        setMovingUp(false);
    }
    
}

void InGame::initUpDownButtons() {
    
    auto psUp=DrawNode::create();
    psUp->setTag(600);
    
    auto visibleSize=Director::getInstance()->getVisibleSize();
    
    
    float x1=visibleSize.width - visibleSize.height * 0.05f;
    float x0=visibleSize.width - visibleSize.height * 0.1f;
    float x2=visibleSize.width;
    
    float y0=visibleSize.height * 0.1f;
    float y1=visibleSize.height * 0.2f;
    
    psUp->drawTriangle(Vec2(x1, 0), Vec2(x0, y0), Vec2(x2, y0), Color4F(0.1f,0.5f,0.6f,1));
    
    auto psDown=DrawNode::create();
    psDown->setTag(601);
    
    psDown->drawTriangle(Vec2(x0, y0), Vec2(x1, y1), Vec2(x2, y0), Color4F(0.1f,0.5f,0.6f,1));
    
    
    //psUp->
    psUp->setContentSize(Size(Vec2(visibleSize.height*0.1, visibleSize.height*0.1)));
    psDown->setContentSize(Size(Vec2(visibleSize.height*0.1, visibleSize.height*0.1)));
    
    auto bbox=psDown->getBoundingBox();
    
    CCLOG("%f,%f,%f,%f\n", bbox.origin.x, bbox.origin.y, bbox.size.width, bbox.size.height);

    
                         
    addChild(psUp, 120);
    addChild(psDown, 120);
    CCLOG("hERE\n");
}

void InGame::initPig() {
    
    auto visibleSize=Director::getInstance()->getVisibleSize();
    
    auto ps=Sprite::create("pig_g.png");
    
    ps->setTag(610); //Pig Tag = 610
    
    ps->setAnchorPoint(Vec2::ZERO);
    ps->setPosition(Vec2(visibleSize.width-0.2*visibleSize.height-ps->getContentSize().width, visibleSize.height * 0.8));
    addChild(ps);
}

void InGame::updateBrick(float t) {
    if(numBricks>0) return;
    
    auto visibleSize=Director::getInstance()->getVisibleSize();
    
    auto pBrick=Sprite::create("brick.png");
    pBrick->setTag(700);
    pBrick->setAnchorPoint(Vec2::ZERO);
    pBrick->setPosition(visibleSize.width-0.2*visibleSize.height-pBrick->getContentSize().width,visibleSize.height*0.9-pBrick->getContentSize().height);
    addChild(pBrick);
    numBricks++;
    holdBrick=false;
    brickFired=false;
}


bool InGame::init() {
    
    if(!Layer::init()) {
        return false;
    }
    
    if(!cacheImages()) return false;
    
    this->schedule(schedule_selector(InGame::updateNewWolf), 4.0f, kRepeatForever, 2.0f);
    this->schedule(schedule_selector(InGame::updateBrick), 4.5f, kRepeatForever, 3.0f);
    this->schedule(schedule_selector(InGame::updateGame));
    
    //Touch setup
    initTouchEvent();
    
    //Init up/Down Buttons
    initUpDownButtons();
    
    //Init Pig
    initPig();
    
    
    //Init wolfInfo's
    numWolfs=0;
    numHungryWolfs=0;
    for(int i=0;i<MaxNumWolfs;++i) wolfs[i].state=-1;
    
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    //Init Fire Button
    auto pBtnFire=MenuItemImage::create(
                                        "fire.png",
                                        "fireSel.png",
                                        CC_CALLBACK_1(InGame::menuFireCallBack, this));
    
    auto menuFire=Menu::create(pBtnFire, NULL);
    menuFire->setOpacity(90);
    menuFire->setPosition(pBtnFire->getContentSize().width * 0.5, pBtnFire->getContentSize().height * 0.5);
    addChild(menuFire);
    
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

