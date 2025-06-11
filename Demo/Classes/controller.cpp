#include "controller.h"
#include "LoadAnimation.h"

USING_NS_CC;

// Static constants
const float Controllers::SPEED_RUN = 200.0f;
const float Controllers::SPEED_JUMP = 400.0f;
const float Controllers::SPEED_DASH = 800.0f;
const float Controllers::DASH_DURATION = 0.2f; // Dash kéo dài 0.2 giây

Controllers* Controllers::createController()
{
    Controllers* controller = new (std::nothrow) Controllers();
    if (controller && controller->init())
    {
        controller->autorelease();
        return controller;
    }
    delete controller;
    return nullptr;
}

bool Controllers::init()
{
    if (!Node::init())
        return false;

    // Initialize member variables
    _player = nullptr;

	_ps = playerState::IDLERIGHT;

    _isWallClingingRight = false;
    _isWallClingingLeft = false;

    // Initialize input state
    _isLeftPressed = false;
    _isRightPressed = false;
    _isJumpPressed = false;
    _isShiftPressed = false;

    // Initialize dash state
    _isDashing = false;
    _dashTimer = 0.0f;
    _dashDirection = 1.0f; // 1 for right, -1 for left

    // Setup input handling
    setupInputHandling();

    scheduleUpdate();



    return true;
}

void Controllers::setPlayer(Node* player)
{
    _player = player;
}

void Controllers::setupInputHandling()
{
    // Tạo keyboard event listener
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(Controllers::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(Controllers::onKeyReleased, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    CCLOG("Input handling setup complete");
}

void Controllers::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    switch (keyCode)
    {
    case EventKeyboard::KeyCode::KEY_A:
    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		_ps = playerState::RUNLEFT;
        _isLeftPressed = true;
        break;

    case EventKeyboard::KeyCode::KEY_D:
    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		_ps = playerState::RUNRIGHT;
        _isRightPressed = true;
        break;

    case EventKeyboard::KeyCode::KEY_SPACE:
    case EventKeyboard::KeyCode::KEY_W:
    case EventKeyboard::KeyCode::KEY_UP_ARROW:
        if (!_isJumpPressed) // Chỉ jump khi mới nhấn, không giữ
        {
            _isJumpPressed = true;
			_ps = (_isLeftPressed) ? playerState::JUMPLEFT : playerState::JUMPRIGHT;
            jump();
        }
        break;

    case EventKeyboard::KeyCode::KEY_SHIFT:
        if (!_isShiftPressed) // Chỉ dash khi mới nhấn shift
        {
            _isShiftPressed = true;
			_ps = (_isLeftPressed) ? playerState::DASHLEFT : playerState::DASHRIGHT;
            dash();
        }
        break;
    }
}

void Controllers::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    switch (keyCode)
    {
    case EventKeyboard::KeyCode::KEY_A:
    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		_ps = playerState::IDLELEFT;
        _isLeftPressed = false;
        break;

    case EventKeyboard::KeyCode::KEY_D:
    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		_ps = playerState::IDLERIGHT;
        _isRightPressed = false;
        break;

    case EventKeyboard::KeyCode::KEY_SPACE:
    case EventKeyboard::KeyCode::KEY_W:
    case EventKeyboard::KeyCode::KEY_UP_ARROW:

        _isJumpPressed = false;
        break;

    case EventKeyboard::KeyCode::KEY_SHIFT:
		_ps = (_isLeftPressed) ? playerState::IDLELEFT : playerState::IDLERIGHT;
        _isShiftPressed = false;
        break;
    }
}

void Controllers::changeAnimationByState()
{
    // Nếu trạng thái không đổi thì không làm gì
    if (_ps == _lastState)
        return;

    _lastState = _ps; // Cập nhật trạng thái hiện tại

    auto sprite = dynamic_cast<cocos2d::Sprite*>(_player);
    if (!sprite)
    {
        CCLOG("Error: Player is not a Sprite");
        return;
    }

    std::string animationName;
    std::string fileName;
    std::string pngName;
    int frameCount;

    bool loop = true;

    switch (_ps)
    {
    case playerState::IDLERIGHT:
        animationName = "IdelRight1";
		fileName = "Idel.plist";
		pngName = "Idel.png";
		frameCount = 23; // Chỉ cần 1 frame cho idle
        break;
    case playerState::IDLELEFT:
        animationName = "idle_left";
        break;
    case playerState::RUNRIGHT:
        animationName = "run_right";
        break;
    case playerState::RUNLEFT:
        animationName = "run_left";
        break;
    case playerState::JUMPRIGHT:
        animationName = "jump_right";
        loop = false;
        break;
    case playerState::JUMPLEFT:
        animationName = "jump_left";
        loop = false;
        break;
    case playerState::DASHRIGHT:
        animationName = "dash_right";
        loop = false;
        break;
    case playerState::DASHLEFT:
        animationName = "dash_left";
        loop = false;
        break;
    }

    // Load animation
    auto animation = LoadAnimation::createAnimation("player.plist", "player.png", 1.0f, Vec2(0.5f, 0.5f), 6, animationName, 0.1f, loop);
    if (!animation)
        return;

    auto animate = LoadAnimation::createAnimateAction(animation);
    if (!animate)
        return;

    sprite->stopAllActions();
    sprite->runAction(animate);
}


void Controllers::moveLeft()
{
    if (!_player || !_player->getPhysicsBody()) return;

    auto body = _player->getPhysicsBody();
    Vec2 velocity = body->getVelocity();
    velocity.x = -SPEED_RUN;
    body->setVelocity(velocity);
}

void Controllers::moveRight()
{
    if (!_player || !_player->getPhysicsBody()) return;

    auto body = _player->getPhysicsBody();
    Vec2 velocity = body->getVelocity();
    velocity.x = SPEED_RUN;
    body->setVelocity(velocity);
}

void Controllers::stopHorizontalMovement()
{
    if (!_player || !_player->getPhysicsBody()) return;

    auto body = _player->getPhysicsBody();
    Vec2 velocity = body->getVelocity();
    velocity.x = 0;
    body->setVelocity(velocity);
}

void Controllers::jump()
{
    if (!_player || !_player->getPhysicsBody()) return;

    // Chỉ jump khi đang trên mặt đất hoặc wall cling
    auto body = _player->getPhysicsBody();
    Vec2 velocity = body->getVelocity();
    velocity.y = SPEED_JUMP;
    body->setVelocity(velocity);

    CCLOG("Player jumped!");
}

void Controllers::dash()
{
    if (!_player || !_player->getPhysicsBody()) return;

    // Xác định hướng dash
    if (_isLeftPressed) {
        _dashDirection = -1.0f;
    }
    else if (_isRightPressed) {
        _dashDirection = 1.0f;
    }
    else {
        // Nếu không nhấn phím nào, dash về phía trước (mặc định)
        _dashDirection = 1.0f;
    }

    // Bắt đầu dash
    _isDashing = true;
    _dashTimer = DASH_DURATION;

    auto body = _player->getPhysicsBody();
    Vec2 velocity = body->getVelocity();
    velocity.x = SPEED_DASH * _dashDirection;
    body->setVelocity(velocity);

    CCLOG("Player dashed! Direction: %f, Velocity: (%f, %f)", _dashDirection, velocity.x, velocity.y);
}

void Controllers::updateDash(float dt)
{
    if (!_isDashing) return;

    _dashTimer -= dt;

    if (_dashTimer <= 0.0f) {
        // Kết thúc dash
        _isDashing = false;
        _dashTimer = 0.0f;
        CCLOG("Dash ended");
    }
}

void Controllers::update(float dt)
{
    if (!_player) return;

    // Update dash state
    updateDash(dt);


    
    
    // Chỉ xử lý movement thường khi KHÔNG đang dash
    if (!_isDashing) {
        // Handle horizontal movement
        if (_isLeftPressed && !_isRightPressed) {
            moveLeft();
        }
        else if (_isRightPressed && !_isLeftPressed) {
            moveRight();
        }
        else if (!_isLeftPressed && !_isRightPressed) {
            stopHorizontalMovement();
        }
    }

    // Cập nhật vertical velocity dựa trên position change
    Vec2 currentPosition = _player->getPosition();
}