#include "PlayerController.h"

USING_NS_CC;

// Static constants
const float PlayerController::SPEED_RUN = 200.0f;
const float PlayerController::SPEED_JUMP = 400.0f;
const float PlayerController::SPEED_DASH = 800.0f;

PlayerController* PlayerController::createController()
{
    PlayerController* controller = new (std::nothrow) PlayerController();
    if (controller && controller->init())
    {
        controller->autorelease();
        return controller;
    }
    delete controller;
    return nullptr;
}

bool PlayerController::init()
{
    if (!Node::init())
        return false;

    // Initialize member variables
    _player = nullptr;
    _currentState = PlayerState::Idle;

    _isGrounded = false;
    _isWallClinging = false;
	_isWallClingingRight = false;
	_isWallClingingLeft = false;

    // Initialize input state
    _isLeftPressed = false;
    _isRightPressed = false;
    _isJumpPressed = false;

    // Initialize flight tracking
    _previousPosition = Vec2::ZERO;
    _verticalVelocity = 0.0f;

    // Setup input handling
    setupInputHandling();

    scheduleUpdate();

    return true;
}

void PlayerController::onEnter()
{
    Node::onEnter();

    // Đăng ký contact listener khi node được add vào scene
    if (_player && _player->getPhysicsBody())
    {
        auto contactListener = EventListenerPhysicsContact::create();
        contactListener->onContactBegin = CC_CALLBACK_1(PlayerController::onContactBegin, this);
        contactListener->onContactSeparate = CC_CALLBACK_1(PlayerController::onContactSeparate, this);

        _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

        CCLOG("Contact listener registered for player");
    }
}

void PlayerController::setPlayer(Node* player)
{
    _player = player;

    // Initialize previous position when player is set
    if (_player)
    {
        _previousPosition = _player->getPosition();
    }
}

bool PlayerController::onContactBegin(PhysicsContact& contact)
{
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();

    // Xác định node nào là player
    Node* otherNode = nullptr;
    PhysicsBody* otherBody = nullptr;

    if (nodeA == _player)
    {
        otherNode = nodeB;
        otherBody = contact.getShapeB()->getBody();
    }
    else if (nodeB == _player)
    {
        otherNode = nodeA;
        otherBody = contact.getShapeA()->getBody();
    }
    else
    {
        return true; // Không phải collision với player
    }

    if (!otherBody) return true;

    // Lấy category bitmask của object khác
    int otherCategory = otherBody->getCategoryBitmask();

    // Kiểm tra loại collision
    Vec2 playerPos = _player->getPosition();
    Vec2 otherPos = otherNode->getPosition();
    Vec2 contactPoint = contact.getContactData()->points[0];

    // Tính vector từ player đến object khác
    Vec2 direction = otherPos - playerPos;

    switch (otherCategory)
    {
    case PhysicsCategory::GROUND:
    {
        // Kiểm tra xem có phải đang đứng trên ground không
        // Collision point phải ở dưới player
        if (contactPoint.y <= playerPos.y - 10) // 10 pixel tolerance
        {
            _isGrounded = true;
            CCLOG("Player is now grounded");

            // Cập nhật state nếu đang rơi
            if (_currentState == PlayerState::JumpingDown)
            {
                _currentState = PlayerState::Idle;
            }
        }
        break;
    }

    case PhysicsCategory::WALL:
    {
        // Kiểm tra collision với tường
        // Sử dụng hướng horizontal để xác định tường bên trái hay phải
        if (abs(direction.x) > abs(direction.y)) // Collision chủ yếu theo phương ngang
        {
            if (direction.x > 0) // Tường bên phải
            {
                _isWallClinging = true;
                _currentState = PlayerState::WallClingingRight;
				_isWallClingingRight = true;
				_isWallClingingLeft = false; // Reset left wall cling state
                CCLOG("Player is clinging to right wall");
            }
            else // Tường bên trái
            {
                _isWallClinging = true;
                _currentState = PlayerState::WallClingingLeft;
				_isWallClingingLeft = true;
				_isWallClingingRight = false; // Reset right wall cling state
                CCLOG("Player is clinging to left wall");
            }
        }
        break;
    }
    }

    return true;
}

void PlayerController::onContactSeparate(PhysicsContact& contact)
{
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();

    // Xác định node nào là player
    Node* otherNode = nullptr;
    PhysicsBody* otherBody = nullptr;

    if (nodeA == _player)
    {
        otherNode = nodeB;
        otherBody = contact.getShapeB()->getBody();
    }
    else if (nodeB == _player)
    {
        otherNode = nodeA;
        otherBody = contact.getShapeA()->getBody();
    }
    else
    {
        return; // Không phải collision với player
    }

    if (!otherBody) return;

    int otherCategory = otherBody->getCategoryBitmask();

    switch (otherCategory)
    {
    case PhysicsCategory::GROUND:
    {
        // Kiểm tra xem player có còn đang chạm ground khác không
        if (!checkStillGrounded())
        {
            _isGrounded = false;
            CCLOG("Player is no longer grounded");

            // Cập nhật state nếu đang rơi
            if (_currentState == PlayerState::Idle)
            {
                _currentState = PlayerState::JumpingDown;
            }
        }
        break;
    }

    case PhysicsCategory::WALL:
    {
        // Kiểm tra xem player có còn đang chạm wall khác không
        if (!checkStillWallClinging())
        {
            _isWallClinging = false;
			_isWallClingingLeft = false;
			_isWallClingingRight = false; // Reset cả hai trạng thái wall cling
            CCLOG("Player is no longer wall clinging");

            // Reset state về idle hoặc jumping
            if (_currentState == PlayerState::WallClingingLeft ||
                _currentState == PlayerState::WallClingingRight)
            {
                _currentState = _isGrounded ? PlayerState::Idle : PlayerState::JumpingDown;
            }
        }
        break;
    }
    }
}

bool PlayerController::checkStillGrounded()
{
    if (!_player || !_player->getPhysicsBody()) return false;

    // Raycast xuống dưới để kiểm tra xem có còn ground không
    Vec2 playerPos = _player->getPosition();
    Vec2 startPoint = playerPos;
    Vec2 endPoint = playerPos + Vec2(0, -15); // Raycast 15 pixel xuống dưới

    auto scene = _player->getScene();
    if (!scene) return false;

    PhysicsRayCastCallbackFunc callback = [this](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) -> bool
        {
            if (info.shape->getBody()->getCategoryBitmask() == PhysicsCategory::GROUND)
            {
                *(bool*)data = true;
                return false; // Dừng raycast
            }
            return true; // Tiếp tục raycast
        };

    bool stillGrounded = false;
    scene->getPhysicsWorld()->rayCast(callback, startPoint, endPoint, &stillGrounded);

    return stillGrounded;
}

bool PlayerController::checkStillWallClinging()
{
    if (!_player || !_player->getPhysicsBody()) return false;

    Vec2 playerPos = _player->getPosition();
    auto scene = _player->getScene();
    if (!scene) return false;

    // Kiểm tra cả hai bên trái và phải
    Vec2 leftPoint = playerPos + Vec2(-20, 0);
    Vec2 rightPoint = playerPos + Vec2(20, 0);

    PhysicsRayCastCallbackFunc callback = [this](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) -> bool
        {
            if (info.shape->getBody()->getCategoryBitmask() == PhysicsCategory::WALL)
            {
                *(bool*)data = true;
                return false; // Dừng raycast
            }
            return true; // Tiếp tục raycast
        };

    bool stillWallLeft = false;
    bool stillWallRight = false;

    scene->getPhysicsWorld()->rayCast(callback, playerPos, leftPoint, &stillWallLeft);
    scene->getPhysicsWorld()->rayCast(callback, playerPos, rightPoint, &stillWallRight);

    return stillWallLeft || stillWallRight;
}

void PlayerController::updatePlayerState()
{
    if (!_player) return;

    // Tự động cập nhật state dựa trên flight status
    if (_isGrounded)
    {
        if (_currentState == PlayerState::JumpingUp || _currentState == PlayerState::JumpingDown)
        {
            _currentState = PlayerState::Idle;
        }
    }
    else // Không grounded
    {
        if (isFlyingUp() && _currentState != PlayerState::JumpingUp)
        {
            _currentState = PlayerState::JumpingUp;
        }
        else if (isFlyingDown() && _currentState != PlayerState::JumpingDown)
        {
            _currentState = PlayerState::JumpingDown;
        }
    }
}

bool PlayerController::isFlyingUp() const
{
    // Kiểm tra player có đang trong không khí và di chuyển lên không
    return !_isGrounded && _verticalVelocity > 10.0f;
}

bool PlayerController::isFlyingDown() const
{
    // Kiểm tra player có đang trong không khí và di chuyển xuống không
    return !_isGrounded && _verticalVelocity < -10.0f;
}

bool PlayerController::isInAir() const
{
    // Kiểm tra player có đang trong không khí không (không chạm đất)
    return !_isGrounded;
}

float PlayerController::getVerticalVelocity() const
{
    return _verticalVelocity;
}

const char* PlayerController::getStateString(PlayerState state)
{
    switch (state)
    {
    case PlayerState::Idle: return "Idle";
    case PlayerState::RunningRight: return "RunningRight";
    case PlayerState::RunningLeft: return "RunningLeft";
    case PlayerState::JumpingUp: return "JumpingUp";
    case PlayerState::JumpingDown: return "JumpingDown";
    case PlayerState::DashingRight: return "DashingRight";
    case PlayerState::DashingLeft: return "DashingLeft";
    case PlayerState::WallClingingLeft: return "WallClingingLeft";
    case PlayerState::WallClingingRight: return "WallClingingRight";
    default: return "Unknown";
    }
}

void PlayerController::setupInputHandling()
{
    // Tạo keyboard event listener
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(PlayerController::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(PlayerController::onKeyReleased, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    CCLOG("Input handling setup complete");
}

void PlayerController::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    switch (keyCode)
    {
    case EventKeyboard::KeyCode::KEY_A:
    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        _isLeftPressed = true;
        break;

    case EventKeyboard::KeyCode::KEY_D:
    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        _isRightPressed = true;
        break;

    case EventKeyboard::KeyCode::KEY_SPACE:
    case EventKeyboard::KeyCode::KEY_W:
    case EventKeyboard::KeyCode::KEY_UP_ARROW:
        if (!_isJumpPressed && _isGrounded || _isWallClinging) // Chỉ jump khi mới nhấn, không giữ
        {
            _isJumpPressed = true;
            jump();
        }
        break;

	case EventKeyboard::KeyCode::KEY_SHIFT:
        if (_isGrounded) {
			_isShiftPressed = true; // Đánh dấu shift được nhấn
			dash();
        }
    }
}

void PlayerController::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    switch (keyCode)
    {
    case EventKeyboard::KeyCode::KEY_A:
    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        _isLeftPressed = false;
        break;

    case EventKeyboard::KeyCode::KEY_D:
    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        _isRightPressed = false;
        break;

    case EventKeyboard::KeyCode::KEY_SPACE:
    case EventKeyboard::KeyCode::KEY_W:
    case EventKeyboard::KeyCode::KEY_UP_ARROW:
        _isJumpPressed = false;
        break;

	case EventKeyboard::KeyCode::KEY_SHIFT:
		_isShiftPressed = false;
    }
}

void PlayerController::moveLeft()
{
    if (!_player || !_player->getPhysicsBody()) return;

    auto body = _player->getPhysicsBody();
    Vec2 velocity = body->getVelocity();
    velocity.x = -SPEED_RUN;
    body->setVelocity(velocity);

    _currentState = PlayerState::RunningLeft;
}

void PlayerController::moveRight()
{
    if (!_player || !_player->getPhysicsBody()) return;

    auto body = _player->getPhysicsBody();
    Vec2 velocity = body->getVelocity();
    velocity.x = SPEED_RUN;
    body->setVelocity(velocity);

    _currentState = PlayerState::RunningRight;
}

void PlayerController::stopHorizontalMovement()
{
    if (!_player || !_player->getPhysicsBody()) return;

    auto body = _player->getPhysicsBody();
    Vec2 velocity = body->getVelocity();
    velocity.x = 0;
    body->setVelocity(velocity);

    if (_isGrounded)
    {
        _currentState = PlayerState::Idle;
    }
}

void PlayerController::jump()
{
    if (!_player || !_player->getPhysicsBody()) return;

    // Chỉ jump khi đang trên mặt đất hoặc wall cling
    if (_isGrounded || _isWallClinging)
    {
        auto body = _player->getPhysicsBody();
        Vec2 velocity = body->getVelocity();
        velocity.y = SPEED_JUMP;
        body->setVelocity(velocity);

        _currentState = PlayerState::JumpingUp;
        _isGrounded = false; // Reset grounded state
        _isWallClinging = false; // Reset wall cling state
		_isWallClingingLeft = false; // Reset left wall cling state
		_isWallClingingRight = false; // Reset right wall cling state

        CCLOG("Player jumped!");
    }
}

void PlayerController::dash() {
	if (!_player || !_player->getPhysicsBody()) return;
	// Chỉ dash khi đang trên mặt đất hoặc wall cling
	if (_isGrounded)
	{
		auto body = _player->getPhysicsBody();
		Vec2 velocity = body->getVelocity();
		if (_isLeftPressed) {
			velocity.x = -SPEED_DASH;
			_currentState = PlayerState::DashingLeft;
		}
		else if (_isRightPressed) {
			velocity.x = SPEED_DASH;
			_currentState = PlayerState::DashingRight;
		}
		else if (!_isLeftPressed && !_isRightPressed){
            velocity.x = SPEED_DASH;
            _currentState = PlayerState::DashingRight;
		}
		body->setVelocity(velocity);
		CCLOG("Player dashed!");
	}
}


void PlayerController::update(float dt)
{
    if (!_player) return;

    // Cập nhật vertical velocity dựa trên position change
    Vec2 currentPosition = _player->getPosition();

    if (dt > 0.0f && _previousPosition != Vec2::ZERO)
    {
        float deltaY = currentPosition.y - _previousPosition.y;
        _verticalVelocity = deltaY / dt;
    }

    // Cập nhật previous position cho frame tiếp theo
    _previousPosition = currentPosition;

    // Cập nhật player state dựa trên movement
    updatePlayerState();

    // Debug output để kiểm tra trạng thái
    static float debugTimer = 0;
    debugTimer += dt;

    if (debugTimer >= 1.0f) // Print debug info mỗi giây
    {
        if (!_player) return;

        // Xử lý input liên tục
        if (_isLeftPressed && _isRightPressed)
        {
            // Cả hai phím được nhấn - dừng lại
            stopHorizontalMovement();
        }
        else if (_isLeftPressed)
        {
            moveLeft();
        }
        else if (_isRightPressed)
        {
            moveRight();
        }
        else
        {
            // Không có phím nào được nhấn - dừng horizontal movement
            stopHorizontalMovement();
        }

        // Cập nhật vertical velocity dựa trên position change
        Vec2 currentPosition = _player->getPosition();

        if (dt > 0.0f && _previousPosition != Vec2::ZERO)
        {
            float deltaY = currentPosition.y - _previousPosition.y;
            _verticalVelocity = deltaY / dt;
        }

        // Cập nhật previous position cho frame tiếp theo
        _previousPosition = currentPosition;

        // Cập nhật player state dựa trên movement
        updatePlayerState();

        // Debug output để kiểm tra trạng thái
        static float debugTimer = 0;
        debugTimer += dt;

        if (debugTimer >= 1.0f) // Print debug info mỗi giây
        {
            if (_isGrounded) {
				_isWallClinging = false; // Reset wall cling state if grounded
				_isWallClingingLeft = false; // Reset left wall cling state
				_isWallClingingRight = false; // Reset right wall cling state
            }
            const char* flightStatus = "Ground";
            if (isFlyingUp()) flightStatus = "Flying UP";
            else if (isFlyingDown()) flightStatus = "Flying DOWN";
            else if (isInAir()) flightStatus = "In Air (stable)";

            CCLOG("Player State - Grounded: %s, Wall Clinging: %s, WallClingingRight: %s, WallClingingLeft: %s, State: %s, Flight: %s, VelY: %.1f",
                _isGrounded ? "YES" : "NO",
                _isWallClinging ? "YES" : "NO",
				_isWallClingingRight ? "YES" : "NO",
				_isWallClingingLeft ? "YES" : "NO",
                getStateString(_currentState),
                flightStatus,
                _verticalVelocity);
            debugTimer = 0;
        }
    }
}