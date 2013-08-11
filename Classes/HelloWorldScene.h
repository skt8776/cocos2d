#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::CCLayerColor
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	HelloWorld();
	virtual ~HelloWorld();
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
	
	void addTarget();
	void spriteMoveFinished(CCNode* sender);
	void gameLogic(float dt);
	void update(float dt);

    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

	// evnet hadnling functions
	void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);

protected:
	cocos2d::CCArray* _targets;
	cocos2d::CCArray* _projectiles;
	cocos2d::CCSprite* player;
};

#endif // __HELLOWORLD_SCENE_H__
