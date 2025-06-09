#include "HelloWorldScene.h"

USING_NS_CC;

bool check = false;


Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);

		addSprites();


        auto contacListener = EventListenerPhysicsContact::create();
        contacListener->onContactBegin = [this](PhysicsContact& contact) -> bool {
            auto nodeA = contact.getShapeA()->getBody()->getNode();
            auto nodeB = contact.getShapeB()->getBody()->getNode();

            sprite1 = dynamic_cast<Sprite*>(nodeA);
            sprite2 = dynamic_cast<Sprite*>(nodeB);

            if (sprite1 && sprite2) {
                sprite2->removeFromParent();
                CCLOG("Collision detected between %s and %s", sprite1->getName().c_str(), sprite2->getName().c_str());
            }
            else {
                CCLOG("Collision detected, but one of the sprites is null or not a Sprite.");
            }

            return true;
            };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(contacListener, this);


       


    //    this->schedule([this](float dt) {
    //        
    //        if (!check) {
    //            //isColliding = sprite1->getBoundingBox().intersectsRect(sprite2->getBoundingBox());
				//auto distance = sprite1->getPosition().distance(sprite2->getPosition());
				//isColliding = (distance < (sprite1->getContentSize().width * sprite1->getScale() + sprite2->getContentSize().width * sprite2->getScale()) / 2);
    //            if (isColliding) {
				//	CCLOG("Sprites are colliding.");
				//	sprite1->stopAllActions();
				//	sprite2->stopAllActions();
				//	check = true;
    //            }
    //            else {
    //                CCLOG("Sprites are not colliding.");
    //            }
    //        }
    //        }, "add_sprites_key");

    
    return true;
}


void HelloWorld::addSprites() {
	auto visibleSize = Director::getInstance()->getVisibleSize();

    sprite1 = Sprite::create("coin.png");
    sprite2 = Sprite::create("coin.png");

	if (!sprite1 || !sprite2) {
		CCLOG("Error loading sprites");
		return;
	}

    sprite1->setScale(0.2f);
    sprite2->setScale(0.2f);

	sprite1->setPosition(Vec2(0, visibleSize.height/2));
	sprite2->setPosition(Vec2(visibleSize.width, visibleSize.height / 2));

	this->addChild(sprite1);
	this->addChild(sprite2);

	sprite1->runAction(MoveBy::create(2, Vec2(600, 0)));
    sprite2->runAction(MoveBy::create(2, Vec2(-600, 0)));
	
}