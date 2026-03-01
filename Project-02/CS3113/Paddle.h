#ifndef PADDLE_H
#define PADDLE_H
#include "constants.h"
#include "Entity.h"

class Ball; // Forward declaration

constexpr float AI_DEADZONE =
    10.0f; // Deadzone for AI paddle movement to prevent jitter

class Paddle : public Entity {
public:
    using Entity::Entity;
    void update(float deltaTime) override;
    void singlePlayerAI(const std::vector<Ball*>& balls, int activeBalls);

private:
    Ball* getClosestBall(const std::vector<Ball*>& balls, int activeBalls);
};

#endif