//
//  InGameScene.h
//  BrotherPigCocos
//
//  Created by fychen on 10/7/14.
//
//

#ifndef __BrotherPigCocos__InGameScene__
#define __BrotherPigCocos__InGameScene__

#include "cocos2d.h"

class InGame : public cocos2d::Layer {
private:
    cocos2d::Texture2D *cachePigG;
    cocos2d::Texture2D *cacheWolfLive;
    cocos2d::Texture2D *cacheWolfDead;
    cocos2d::Texture2D *cacheWolfRun;
    cocos2d::Texture2D *cacheBallon;
    static const int MaxNumWolfs=20;
    static const int MaxHungryWolvs=4;
    
    struct wolfInfo{
        float x,y;
        int tag;
        char state;
        cocos2d::Sprite *pSprite;
        cocos2d::Sprite *pBallon;
    };
    
    wolfInfo wolfs[MaxNumWolfs];
    int numWolfs;
    int numHungryWolfs;
    void genWolf(int k);
    bool cacheImages();
    void updateNewWolf(float t);
    void updateGame(float t);
    void touchGroundWolf(cocos2d::Node* node);
    void touchGroundBallon(cocos2d::Node* node);
    void touchWall(cocos2d::Node* node);
    void attackWolf(cocos2d::Node* node);
    void holdWolf(cocos2d::Node* node);
    void GameOver();
    
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    //selector call back for start menu item
    void menuResumeCallback(cocos2d::Ref* pSender);
    
    void menuPauseCallback(cocos2d::Ref* pSender);
    
    void menuExitCallback(cocos2d::Ref* pSender);
    
    void menuRestartCallBack(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(InGame);
};

#endif /* defined(__BrotherPigCocos__InGameScene__) */
