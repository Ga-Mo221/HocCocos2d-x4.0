#ifndef __PLAYER_CONTROLLER_H__
#define __PLAYER_CONTROLLER_H__

#include "cocos2d.h"

class PlayerController : public cocos2d::Node
{
public:
    // Factory method
    static PlayerController* createController();

    // Initialization
    virtual bool init() override;
    virtual void onEnter() override;

    // Player setup
    void setPlayer(cocos2d::Node* player);

    // Collision handling
    bool onContactBegin(cocos2d::PhysicsContact& contact);
    void onContactSeparate(cocos2d::PhysicsContact& contact);

    // State checking
    bool isGrounded() const { return _isGrounded; }
    bool isWallClinging() const { return _isWallClinging; }

    // Flight checking
    bool isFlyingUp() const;
    bool isFlyingDown() const;
    bool isInAir() const;
    float getVerticalVelocity() const;

    // Update method
    virtual void update(float dt) override;

private:
    // Input handling
    void setupInputHandling();
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    // Movement methods
    void moveLeft();
    void moveRight();
    void stopHorizontalMovement();
    void jump();
    void dash();

    // Input state tracking
    bool _isLeftPressed;
    bool _isRightPressed;
    bool _isJumpPressed;
    bool _isShiftPressed;


    // Helper methods for collision detection
    bool checkStillGrounded();
    bool checkStillWallClinging();

    // Helper methods for state management
    void updatePlayerState();

    

    // Physics constants
    static const float SPEED_RUN;
    static const float SPEED_JUMP;
    static const float SPEED_DASH;

    // Physics categories
    enum PhysicsCategory
    {
        GROUND = 0x01,
        WALL = 0x02,
        PLAYER = 0x03
    };

    // Player states
    enum class PlayerState
    {
        Idle,
        RunningRight,
        RunningLeft,
        JumpingUp,
        JumpingDown,
        DashingRight,
        DashingLeft,
        WallClingingLeft,
        WallClingingRight
    };

    // Helper method for debug
    const char* getStateString(PlayerState state);

    // Member variables
    cocos2d::Node* _player;
    PlayerState _currentState;

    // Physics state
    bool _isGrounded;
    bool _isWallClinging;

	bool _isWallClingingLeft;
	bool _isWallClingingRight;

    // Flight tracking
    cocos2d::Vec2 _previousPosition;
    float _verticalVelocity;
};

#endif // __PLAYER_CONTROLLER_H__