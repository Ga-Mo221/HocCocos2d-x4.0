#ifndef __CONTROLLERS_H__
#define __CONTROLLERS_H__

#include "cocos2d.h"

class Controllers : public cocos2d::Node
{
public:
    // Factory method
    static Controllers* createController();

    // Initialization
    virtual bool init() override;

    // Player setup
    void setPlayer(cocos2d::Node* player);

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
    void updateDash(float dt);

    void changeAnimationByState();

	

	enum class playerState
	{
		IDLERIGHT,
		IDLELEFT,
		RUNRIGHT,
		RUNLEFT,
		JUMPRIGHT,
		JUMPLEFT,
		DASHRIGHT,
		DASHLEFT
	};

	playerState _ps;
    playerState _lastState = playerState::JUMPRIGHT;



    // Input state tracking
    bool _isLeftPressed;
    bool _isRightPressed;
    bool _isJumpPressed;
    bool _isShiftPressed;

    // Dash state tracking
    bool _isDashing;
    float _dashTimer;
    float _dashDirection; // 1.0f for right, -1.0f for left

    // Physics constants
    static const float SPEED_RUN;
    static const float SPEED_JUMP;
    static const float SPEED_DASH;
    static const float DASH_DURATION;

    // Member variables
    cocos2d::Node* _player;

    bool _isWallClingingLeft;
    bool _isWallClingingRight;
};

#endif // __CONTROLLERS_H__