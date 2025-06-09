#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Scene
{
private:
    cocos2d::Sprite* ground;
    cocos2d::Sprite* ball;
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    void addSprite();

    cocos2d::Sprite* createpolygon(
        const std::string& filename,
        cocos2d::Vec2 pos,
        float scale,
        const std::vector<cocos2d::Vec2>& diem,
        int sodiem,
        cocos2d::Node* parent);


    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
