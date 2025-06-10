#include "MenuScene.h"
#include "ChangeScene.h"
#include "GameScene1.h"

USING_NS_CC;

Scene* MenuScene::createScene()
{
    return MenuScene::create();
}


bool MenuScene::init()
{
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto label = Label::createWithTTF("Scene Menu", "fonts/Marker Felt.ttf", 24);
    label->setPosition(Vec2(origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);


    // Thêm nút chuyển scene
    auto menuItem = MenuItemLabel::create(
        Label::createWithTTF("Start Game", "fonts/Marker Felt.ttf", 36),
        [](Ref* sender) {
            ChangeScene::ChangeScenes(GameScene1::createScene());
        }
    );

    menuItem->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));

    auto menu = Menu::create(menuItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
    

    return true;
}
