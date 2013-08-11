#include <cstdlib>

#include "HelloWorldScene.h"
#include "AppMacros.h"

USING_NS_CC;

HelloWorld::HelloWorld()
	:_targets(nullptr),
	_projectiles(nullptr)
{


}

HelloWorld::~HelloWorld()
{
	if(_targets)
	{
		_targets->release();
		_targets = nullptr;

	}

	if(_projectiles)
	{
		_projectiles->release();
		_projectiles = nullptr;

	}
}

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

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
    if ( !CCLayerColor::initWithColor(ccc4(255, 255, 255, 255)) )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);
	

    /////////////////////////////
    // 3. add your codes below...

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	player = CCSprite::create("undine.png");
	player->setPosition(CCPoint(winSize.width / 2, winSize.height / 2));

	this->addChild(player);

	this->schedule(schedule_selector(HelloWorld::gameLogic), 1.0f);
	this->schedule(schedule_selector ( HelloWorld::update ));

    this->setTouchEnabled(true);
	_targets = new CCArray;
	_projectiles = new CCArray;

    return true;
}

void HelloWorld::addTarget() {
	CCSprite* target = CCSprite::create("changeling.png");
	CCSize size = target->getContentSize();

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	int minY = target->getContentSize().height / 2;
	int maxY = winSize.height - target->getContentSize().height / 2;
	int rangeY = maxY - minY;
	int actualY = winSize.height / 2;

	target->setPosition(
		CCPoint(winSize.width + (target->getContentSize().width/2), actualY));
	this->addChild(target);

	float duration = 3.0f;

	CCFiniteTimeAction* actionMove = CCMoveTo::create(duration, CCPoint(0 - target->getContentSize().width / 2, actualY));
	CCFiniteTimeAction* actionMoveDone = CCCallFuncN::create(this, callfuncN_selector(HelloWorld::spriteMoveFinished));

	target->runAction(CCSequence::create(actionMove, actionMoveDone, nullptr));

	target->setTag(1);
	_targets->addObject(target);
}

void HelloWorld::spriteMoveFinished(CCNode* sender) {
	CCSprite *sprite = (CCSprite *) sender;
	this->removeChild(sprite, true);
	switch(sprite->getTag())
	{
	case 1:
		_targets->removeObject(sprite);
	case 2:
		_projectiles->removeObject(sprite);
		break;
	default:
		break;
	}
}

void HelloWorld::gameLogic(float dt) {
	this->addTarget();
}



void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
	CCTouch* touch = (CCTouch*)(touches->anyObject());
	CCPoint location = touch->getLocation();

	// Set up initial location of projectile
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCSprite *projectile = CCSprite::create("changeling.png");
	projectile->setPosition( ccp(20, winSize.height/2) );

	// Determine offset of location to projectile
	int offX = location.x - projectile->getPosition().x;
	int offY = location.y - projectile->getPosition().y;

	// Bail out if we are shooting down or backwards
	if (offX <= 0) return;

	// Ok to add now - we've double checked position
	this->addChild(projectile);

	// Determine where we wish to shoot the projectile to
	int realX = winSize.width + (projectile->getContentSize().width/2);
	float ratio = (float)offY / (float)offX;
	int realY = (realX * ratio) + projectile->getPosition().y;
	CCPoint realDest = ccp(realX, realY);

	// Determine the length of how far we're shooting
	int offRealX = realX - projectile->getPosition().x;
	int offRealY = realY - projectile->getPosition().y;
	float length = sqrtf((offRealX * offRealX) + (offRealY*offRealY));
	float velocity = 480/1; // 480pixels/1sec
	float realMoveDuration = length/velocity;

	// Move projectile to actual endpoint
	CCFiniteTimeAction * moved = CCMoveTo::create(realMoveDuration, realDest);
	CCFiniteTimeAction * finished = CCCallFuncN::create(this, 
		callfuncN_selector(HelloWorld::spriteMoveFinished));

	projectile->runAction( CCSequence::create(moved, finished, nullptr) );

	projectile->setTag(2);
	_projectiles->addObject(projectile);
}

void HelloWorld::update(float dt)
{
	CCArray* projectilesToDelete = new CCArray;
	CCObject* jt = NULL;
	CCObject* it = NULL;
	CCARRAY_FOREACH(_projectiles, it)
	{
		CCSprite* projectile = static_cast<CCSprite*>(it);
		CCRect projectileRect = CCRectMake(
			projectile->getPosition().x - (projectile->getContentSize().width/2),
			projectile->getPosition().y - (projectile->getContentSize().height/2),
			projectile->getContentSize().width,
			projectile->getContentSize().height);

		CCArray* targetsToDelete =new CCArray;

		// for (jt = _targets->begin(); jt != _targets->end(); jt++)
		CCARRAY_FOREACH(_targets, jt)
		{
			CCSprite *target = dynamic_cast<CCSprite*>(jt);
			CCRect targetRect = CCRectMake(
				target->getPosition().x - (target->getContentSize().width/2),
				target->getPosition().y - (target->getContentSize().height/2),
				target->getContentSize().width,
				target->getContentSize().height);

			// if (CCRect::CCRectIntersectsRect(projectileRect, targetRect))
			if (projectileRect.intersectsRect(targetRect))
			{
				targetsToDelete->addObject(target);
			}
		}

		CCARRAY_FOREACH(targetsToDelete, jt)
		{

			CCSprite* target = static_cast<CCSprite*>(jt);
			_targets->removeObject(target);
			this->removeChild(target, true);
		}

		if(targetsToDelete->count() > 0)
		{
			projectilesToDelete->addObject(projectile);
		}
		targetsToDelete->release();
	}
		CCARRAY_FOREACH(projectilesToDelete, it)
		{
			CCSprite* projectile = dynamic_cast<CCSprite*>(it);
			_projectiles->removeObject(projectile);
			this->removeChild(projectile, true);
		}
		projectilesToDelete->release();

	
}