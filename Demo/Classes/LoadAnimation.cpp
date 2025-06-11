#include "LoadAnimation.h"

USING_NS_CC;

bool LoadAnimation::loadSpriteFrameCache(const std::string& plistFile, const std::string& pngFile)
{
    // Kiểm tra xem file có tồn tại không
    if (!FileUtils::getInstance()->isFileExist(plistFile))
    {
        CCLOG("Error: Plist file not found: %s", plistFile.c_str());
        return false;
    }

    if (!FileUtils::getInstance()->isFileExist(pngFile))
    {
        CCLOG("Error: PNG file not found: %s", pngFile.c_str());
        return false;
    }

    // Load sprite frame cache
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistFile, pngFile);
    CCLOG("Loaded sprite frames from: %s, %s", plistFile.c_str(), pngFile.c_str());

    return true;
}

Animation* LoadAnimation::createAnimation(
    const std::string& plistFile,
    const std::string& pngFile,
    float scale,
    const Vec2& anchorPoint,
    int frameCount,
    const std::string& animationName,
    float frameDelay,
    bool isLoop)
{
    // Load sprite frame cache
    if (!loadSpriteFrameCache(plistFile, pngFile))
    {
        return nullptr;
    }

    // Tạo vector chứa các sprite frame
    Vector<SpriteFrame*> frames;

    // Thử load frame theo pattern thông thường
    for (int i = 0; i < frameCount; i++)
    {
        // Thử nhiều pattern tên file khác nhau
        std::vector<std::string> patterns = {
            StringUtils::format("%s_%d.png", animationName.c_str(), i),
            StringUtils::format("%s%d.png", animationName.c_str(), i),
            StringUtils::format("%s_%02d.png", animationName.c_str(), i),
            StringUtils::format("%s%02d.png", animationName.c_str(), i),
            StringUtils::format("frame_%d.png", i),
            StringUtils::format("frame%d.png", i),
            StringUtils::format("frame_%02d.png", i),
            StringUtils::format("frame%02d.png", i)
        };

        SpriteFrame* frame = nullptr;

        // Thử từng pattern cho đến khi tìm thấy
        for (const auto& pattern : patterns)
        {
            frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(pattern);
            if (frame != nullptr)
            {
                CCLOG("Found frame: %s", pattern.c_str());
                break;
            }
        }

        if (frame != nullptr)
        {
            frames.pushBack(frame);
        }
        else
        {
            CCLOG("Warning: Could not find frame %d for animation %s", i, animationName.c_str());
        }
    }

    // Kiểm tra xem có frame nào được tìm thấy không
    if (frames.empty())
    {
        CCLOG("Error: No frames found for animation %s", animationName.c_str());
        return nullptr;
    }

    // Tạo animation
    Animation* animation = Animation::createWithSpriteFrames(frames, frameDelay);
    if (animation == nullptr)
    {
        CCLOG("Error: Failed to create animation %s", animationName.c_str());
        return nullptr;
    }

    // Set loop
    animation->setLoops(isLoop ? -1 : 1); // -1 = infinite loop, 1 = play once

    // Retain animation để tránh bị giải phóng
    animation->retain();

    CCLOG("Successfully created animation %s with %d frames", animationName.c_str(), (int)frames.size());

    return animation;
}

Animation* LoadAnimation::createAnimationWithPattern(
    const std::string& plistFile,
    const std::string& pngFile,
    const std::string& frameNamePattern,
    int startFrame,
    int endFrame,
    float scale,
    const Vec2& anchorPoint,
    float frameDelay,
    bool isLoop)
{
    // Load sprite frame cache
    if (!loadSpriteFrameCache(plistFile, pngFile))
    {
        return nullptr;
    }

    // Tạo vector chứa các sprite frame
    Vector<SpriteFrame*> frames;

    for (int i = startFrame; i <= endFrame; i++)
    {
        std::string frameName = StringUtils::format(frameNamePattern.c_str(), i);
        SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);

        if (frame != nullptr)
        {
            frames.pushBack(frame);
            CCLOG("Added frame: %s", frameName.c_str());
        }
        else
        {
            CCLOG("Warning: Could not find frame: %s", frameName.c_str());
        }
    }

    // Kiểm tra xem có frame nào được tìm thấy không
    if (frames.empty())
    {
        CCLOG("Error: No frames found with pattern %s", frameNamePattern.c_str());
        return nullptr;
    }

    // Tạo animation
    Animation* animation = Animation::createWithSpriteFrames(frames, frameDelay);
    if (animation == nullptr)
    {
        CCLOG("Error: Failed to create animation with pattern %s", frameNamePattern.c_str());
        return nullptr;
    }

    // Set loop
    animation->setLoops(isLoop ? -1 : 1);

    // Retain animation
    animation->retain();

    CCLOG("Successfully created animation with pattern %s, %d frames", frameNamePattern.c_str(), (int)frames.size());

    return animation;
}


Animate* LoadAnimation::createAnimateAction(Animation* animation)
{
    if (animation == nullptr)
    {
        CCLOG("Error: Animation is null");
        return nullptr;
    }

    Animate* animate = Animate::create(animation);
    if (animate == nullptr)
    {
        CCLOG("Error: Failed to create Animate action");
        return nullptr;
    }

    CCLOG("Successfully created Animate action");
    return animate;
}

Sprite* LoadAnimation::createSpriteWithFrame(
    const std::string& plistFile,
    const std::string& pngFile,
    const std::string& frameName,
    float scale,
    const Vec2& anchorPoint)
{
    // Load sprite frame cache
    if (!loadSpriteFrameCache(plistFile, pngFile))
    {
        return nullptr;
    }

    // Lấy sprite frame
    SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
    if (frame == nullptr)
    {
        CCLOG("Error: Could not find frame: %s", frameName.c_str());
        return nullptr;
    }

    // Tạo sprite
    Sprite* sprite = Sprite::createWithSpriteFrame(frame);
    if (sprite == nullptr)
    {
        CCLOG("Error: Failed to create sprite with frame: %s", frameName.c_str());
        return nullptr;
    }

    // Set scale và anchor point
    sprite->setScale(scale);
    sprite->setAnchorPoint(anchorPoint);

    CCLOG("Successfully created sprite with frame: %s", frameName.c_str());

    return sprite;
}