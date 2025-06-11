#include "GameScene1.h"
#include "controller.h"
#include "LoadAnimation.h"

USING_NS_CC;

Scene* GameScene1::createScene()
{
    auto scene = Scene::createWithPhysics();
    auto layer = GameScene1::create();

    // Enable physics debug drawing (remove in production)
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    scene->getPhysicsWorld()->setGravity(Vec2(0, -980)); // Set gravity

    scene->addChild(layer);
    return scene;
}

bool GameScene1::init()
{
    if (!Scene::init())
    {
        return false;
    }

    // Initialize member variables
    _tileMap = nullptr;
    _player = nullptr;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Create title label
    auto label = Label::createWithTTF("Map1", "fonts/Marker Felt.ttf", 24);
    if (label)
    {
        label->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - label->getContentSize().height));
        this->addChild(label, 1);
    }

    // Load and setup tile map
    if (!loadTileMap())
    {
        CCLOG("Failed to load tile map");
        return false;
    }

    // Create player
    if (!createPlayer())
    {
        CCLOG("Failed to create player");
        return false;
    }

    return true;
}

bool GameScene1::loadTileMap()
{
    _tileMap = TMXTiledMap::create("Map1.tmx");
    if (!_tileMap)
    {
        CCLOG("Failed to load Map1.tmx");
        return false;
    }

    _tileMap->setScale(0.8f); // Scale down map for better visibility
    this->addChild(_tileMap);

    // Create physics bodies from tile map
    return createPhysicsFromTileMap(_tileMap);
}

bool GameScene1::createPhysicsFromTileMap(TMXTiledMap* map)
{
    auto layer = map->getLayer("Ground");
    if (!layer)
    {
        CCLOG("Ground layer not found in tile map");
        return false;
    }

    auto tileSize = map->getTileSize();
    auto mapSize = map->getMapSize();

    // Create physics bodies for collidable tiles
    for (int x = 0; x < mapSize.width; ++x)
    {
        for (int y = 0; y < mapSize.height; ++y)
        {
            int tileGid = layer->getTileGIDAt(Vec2(x, y));
            if (tileGid == 0) continue; // Skip empty tiles

            Value prop = map->getPropertiesForGID(tileGid);
            if (prop.isNull() || prop.getType() != Value::Type::MAP) continue;

            auto vm = prop.asValueMap();
            if (vm.count("collidable") && vm.at("collidable").asBool())
            {
                auto body = PhysicsBody::createBox(tileSize);
                body->setDynamic(false);
                body->setCategoryBitmask(0x01);          // Ground category
                body->setCollisionBitmask(0xFFFFFFFF);   // Collide with everything
                body->setContactTestBitmask(0xFFFFFFFF); // Generate contact events

                auto node = Node::create();
                float px = x * tileSize.width + tileSize.width / 2;
                float py = (mapSize.height - y - 1) * tileSize.height + tileSize.height / 2;
                node->setPosition(Vec2(px, py));
                node->setPhysicsBody(body);
                this->addChild(node);
            }
            if (vm.count("Wal") && vm.at("Wal").asBool())
            {
                auto body = PhysicsBody::createBox(tileSize);
                body->setDynamic(false);
                body->setCategoryBitmask(0x02);          // Ground category
                body->setCollisionBitmask(0xFFFFFFFF);   // Collide with everything
                body->setContactTestBitmask(0xFFFFFFFF); // Generate contact events

                auto node = Node::create();
                float px = x * tileSize.width + tileSize.width / 2;
                float py = (mapSize.height - y - 1) * tileSize.height + tileSize.height / 2;
                node->setPosition(Vec2(px, py));
                node->setPhysicsBody(body);
                this->addChild(node);
            }
        }
    }

    return true;
}

Vec2 GameScene1::getSpawnPosition(TMXTiledMap* map)
{
    auto objectGroup = map->getObjectGroup("Objects");
    if (!objectGroup)
    {
        CCLOG("Object group 'Objects' not found in the map");
        return Vec2(100, 100); // Default spawn position
    }

    auto objects = objectGroup->getObjects();
    for (const auto& obj : objects)
    {
        ValueMap dict = obj.asValueMap();
        std::string name = dict["name"].asString();
        std::string kind = dict["Kind"].asString();

        if (kind == "Player1" || name == "player")
        {
            float x = dict["x"].asFloat();
            float y = dict["y"].asFloat();

            CCLOG("Found spawn point at: (%f, %f)", x, y);
            return Vec2(x, y);
        }
    }

    CCLOG("No spawn point found, using default position");
    return Vec2(100, 100); // Default spawn position
}

bool GameScene1::createPlayer()
{
    Vec2 spawnPos = getSpawnPosition(_tileMap);

    // Tạo player sprite với kiểm tra chặt chẽ
    auto player = LoadAnimation::createSpriteWithFrame(
        "Idel.plist", "Idel.png", "IdelRight1.png",
        0.5f, Vec2(0.5f, 0.5f));

    if (!player)
    {
        CCLOG("❌ CRITICAL: Failed to create player sprite - files missing!");
        // Tạo sprite backup đơn giản
        player = Sprite::create();
        if (!player) {
            CCLOG("❌ Even basic sprite creation failed!");
            return false;
        }
    }

    // Tạo physics body...
    Vec2 trianglePoints[] = {
		Vec2(-60, 25), Vec2(40, 25), Vec2(40, -90), Vec2(-60, -90)
    };

    auto body = PhysicsBody::createPolygon(trianglePoints, 4,
        PhysicsMaterial(0.1f, 0.0f, 0.7f));

    if (!body) {
        CCLOG("❌ Failed to create physics body");
        return false;
    }

    body->setDynamic(true);
    body->setRotationEnable(false);
    body->setCategoryBitmask(0x03);
    body->setCollisionBitmask(0xFFFFFFFF);
    body->setContactTestBitmask(0xFFFFFFFF);
    body->setMass(1.0f);

    player->setPhysicsBody(body);
    player->setPosition(spawnPos);

    // ✅ Kiểm tra trước khi add
    if (player) {
        this->addChild(player);
        _player = player;
        CCLOG("✅ Player created successfully");
    }

    // Tạo controller với kiểm tra
    auto controller = Controllers::createController();
    if (controller) {
        controller->setPlayer(_player);
        this->addChild(controller);
        CCLOG("✅ Controller created successfully");
    }
    else {
        CCLOG("⚠️ Controller creation failed - continuing without controller");
    }

    return true;
}