#ifndef __GAME_SCENE_1_H__
#define __GAME_SCENE_1_H__

#include "cocos2d.h"

class GameScene1 : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init() override;

    CREATE_FUNC(GameScene1);

private:
    // Helper methods
    bool loadTileMap();
    bool createPhysicsFromTileMap(cocos2d::TMXTiledMap* map);
    bool createPlayer();
    cocos2d::Vec2 getSpawnPosition(cocos2d::TMXTiledMap* map);

    // Member variables
    cocos2d::TMXTiledMap* _tileMap;
    cocos2d::Node* _player;
};

#endif // __GAME_SCENE_1_H__