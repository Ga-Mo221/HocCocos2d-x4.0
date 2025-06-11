#ifndef __LOAD_ANIMATION_H__
#define __LOAD_ANIMATION_H__

#include "cocos2d.h"

class LoadAnimation
{
public:
    /**
     * Tạo animation từ file plist và png
     * @param plistFile Tên file plist (ví dụ: "player_animation.plist")
     * @param pngFile Tên file png (ví dụ: "player_animation.png")
     * @param scale Tỷ lệ scale của sprite (mặc định: 1.0f)
     * @param anchorPoint Điểm neo của sprite (mặc định: Vec2(0.5f, 0.5f))
     * @param frameCount Số lượng frame trong animation
     * @param animationName Tên animation để tạo (mặc định: "default_animation")
     * @param frameDelay Thời gian delay giữa các frame (mặc định: 0.1f)
     * @param isLoop Animation có lặp lại không (mặc định: true)
     * @return Animation object, nullptr nếu thất bại
     */
    static cocos2d::Animation* createAnimation(
        const std::string& plistFile,
        const std::string& pngFile,
        float scale = 1.0f,
        const cocos2d::Vec2& anchorPoint = cocos2d::Vec2(0.5f, 0.5f),
        int frameCount = 1,
        const std::string& animationName = "default_animation",
        float frameDelay = 0.1f,
        bool isLoop = true
    );

    /**
     * Tạo animation từ file plist và png với tên frame pattern
     * @param plistFile Tên file plist
     * @param pngFile Tên file png
     * @param frameNamePattern Pattern tên frame (ví dụ: "player_run_%d.png")
     * @param startFrame Frame bắt đầu (thường là 0 hoặc 1)
     * @param endFrame Frame kết thúc
     * @param scale Tỷ lệ scale
     * @param anchorPoint Điểm neo
     * @param frameDelay Thời gian delay giữa các frame
     * @param isLoop Animation có lặp lại không
     * @return Animation object, nullptr nếu thất bại
     */
    static cocos2d::Animation* createAnimationWithPattern(
        const std::string& plistFile,
        const std::string& pngFile,
        const std::string& frameNamePattern,
        int startFrame,
        int endFrame,
        float scale = 1.0f,
        const cocos2d::Vec2& anchorPoint = cocos2d::Vec2(0.5f, 0.5f),
        float frameDelay = 0.1f,
        bool isLoop = true
    );

    

    /**
     * Tạo Animate action từ Animation
     * @param animation Animation object
     * @return Animate action, nullptr nếu thất bại
     */
    static cocos2d::Animate* createAnimateAction(cocos2d::Animation* animation);

    /**
     * Tạo sprite với frame đầu tiên của animation
     * @param plistFile Tên file plist
     * @param pngFile Tên file png
     * @param frameName Tên frame đầu tiên
     * @param scale Tỷ lệ scale
     * @param anchorPoint Điểm neo
     * @return Sprite object, nullptr nếu thất bại
     */
    static cocos2d::Sprite* createSpriteWithFrame(
        const std::string& plistFile,
        const std::string& pngFile,
        const std::string& frameName,
        float scale = 1.0f,
        const cocos2d::Vec2& anchorPoint = cocos2d::Vec2(0.5f, 0.5f)
    );

private:
    /**
     * Load sprite frame cache từ plist và png
     * @param plistFile Tên file plist
     * @param pngFile Tên file png
     * @return true nếu thành công, false nếu thất bại
     */
    static bool loadSpriteFrameCache(const std::string& plistFile, const std::string& pngFile);
};

#endif // __LOAD_ANIMATION_H__