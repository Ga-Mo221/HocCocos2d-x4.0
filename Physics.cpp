#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    auto scene = Scene::createWithPhysics();
    auto layer = HelloWorld::create();
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    scene->addChild(layer);
    return scene;
}


bool HelloWorld::init()
{
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);


    addSprite();


    return true;
}


void HelloWorld::addSprite() {
    auto ground = Sprite::create("ground.png");

    auto groundBody = PhysicsBody::createBox(ground->getContentSize());
    groundBody->setDynamic(false);  //static ground
    ground->setPhysicsBody(groundBody);
    ground->setAnchorPoint(Vec2(0,0));
    ground->setScaleX(3);
    ground->setScaleY(0.5);
    ground->setRotation(0);
    this->addChild(ground);

    auto ball = Sprite::create("coin.png");
    auto ballBody = PhysicsBody::createCircle(ball->getContentSize().width / 2, 
        PhysicsMaterial(1.0f, 0.3f, 100.0f));

    ballBody->applyImpulse(Vec2(0, -1000000000));
    ball->setPhysicsBody(ballBody); //Dynamic ball
    ball->setPosition(Vec2(800, 400));
    ball->setScale(0.1);
    this->addChild(ball);


    std::vector<cocos2d::Vec2> vertices = {Vec2(-600, -600), Vec2(600, -600) , Vec2(600, 600) , Vec2(-600, 600)};
    createpolygon("coin.png", Vec2(100, 600), 0.1f, vertices, 4, this);

    groundBody->setContactTestBitmask(true);
    groundBody->setTag(1); // Tag = 1 cho ground

    ballBody->setContactTestBitmask(true);
    ballBody->setTag(2); // Tag = 2 cho ball


    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = [](PhysicsContact& contact) {
        auto bodyA = contact.getShapeA()->getBody();
        auto bodyB = contact.getShapeB()->getBody();

        int tagA = bodyA->getTag();
        int tagB = bodyB->getTag();

        // Kiểm tra nếu một trong hai là ground và một là ball
        if ((tagA == 1 && tagB == 2) || (tagA == 2 && tagB == 1)) {
            CCLOG("Ball chạm vào ground!");
        }

        return true; // Cho phép xử lý va chạm tiếp tục
        };

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);
}


cocos2d::Sprite* HelloWorld::createpolygon(
    const std::string& filename,
    cocos2d::Vec2 pos,
    float scale,
    const std::vector<cocos2d::Vec2>& diem,
    int sodiem,
    cocos2d::Node* parent)
{
    auto sprite = cocos2d::Sprite::create(filename);
    sprite->setScale(scale);
    sprite->setPosition(pos);

    // tạo PhysicsBody từ mảng điểm
    auto polygonBody = cocos2d::PhysicsBody::createPolygon(diem.data(), sodiem);
    sprite->setPhysicsBody(polygonBody);

    // thêm vào node cha
    parent->addChild(sprite);


    auto listener = cocos2d::EventListenerMouse::create();
    listener->onMouseDown = [sprite](cocos2d::EventMouse* event) {
        auto locationInNode = sprite->convertToNodeSpace(event->getLocation());
        auto spriteSize = sprite->getContentSize();
        cocos2d::Rect rect(0, 0, spriteSize.width, spriteSize.height);

        if (rect.containsPoint(locationInNode)) {
            CCLOG("da clik");
            auto pos = sprite->getPosition();
            sprite->setPositionY(pos.y + 100);  // Di chuyển lên 100 pixel
        }
        };

    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, sprite);


    return sprite;
}