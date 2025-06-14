#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

	void addSprites();

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
	cocos2d::Sprite* sprite1;
    cocos2d::Sprite* sprite2;
    bool isColliding;

};

#endif // __HELLOWORLD_SCENE_H__
