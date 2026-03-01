#ifndef BALL_H
#define BALL_H
#include "constants.h"
#include "Entity.h"

class Paddle;

class Ball : public Entity {
public:
    using Entity::Entity;
    void update(float deltaTime, Paddle* leftPaddle, Paddle* rightPaddle,
                int& leftScore, int& rightScore);
    void reset();

    void setBaseSpeed(float speed) { mBaseSpeed = speed; }

    void setScale(Vector2 scale) {
        Entity::setScale(scale);
        mRadius = scale.x / 2.0f;
    }

    static constexpr int SLOW_SPEED = 100; // 67 mode
    static constexpr float FAST_SPEED = 250; // 1-3 balls

private:
    float sweepCollision(const Paddle* paddle, float deltaTime,
                         Vector2& outNormal) const;
    void resolveCollision(Paddle* const paddle, Vector2 normal);
    void depenetrate(const Paddle* paddle);

    float mSpeedMultiplier = 1.0f;
    float mBaseSpeed = FAST_SPEED;
    float mRadius = mScale.x / 2.0f;
    Paddle* lastCollision = nullptr;
};

#endif // BALL_H