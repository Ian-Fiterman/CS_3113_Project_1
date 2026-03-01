#include "Ball.h"
#include "Paddle.h"

/**
 * @brief Clamps paddle position to screen edges and resets movement
 * @param deltaTime
 */
void Paddle::update(float deltaTime) {
    Entity::update(deltaTime);
    float halfHeight = mScale.y / 2.0f;
    // Clamp to screen edges
    mPosition.y = clamp(mPosition.y, halfHeight, SCREEN_HEIGHT - halfHeight);
    resetMovement();
}

/**
 * @brief Simple AI for single-player mode, moves paddle towards closest ball
 * @param balls the vector of balls to track
 * @param activeBalls the number of active balls in the vector
 */
void Paddle::singlePlayerAI(const std::vector<Ball*>& balls, int activeBalls) {
    Ball* closestBall = getClosestBall(balls, activeBalls);
    float ballY = closestBall->getPosition().y;
    float paddleY = mPosition.y;
    if (ballY < paddleY - AI_DEADZONE) {
        moveUp();
    } else if (ballY > paddleY + AI_DEADZONE) {
        moveDown();
    }
}

/**
 * @brief Finds the closest ball to the paddle based on horizontal distance
 * @param balls the vector of balls to check
 * @param activeBalls the number of active balls in the vector
 * @return a pointer to the closest ball
 */
Ball* Paddle::getClosestBall(const std::vector<Ball*>& balls, int activeBalls) {
    Ball* closestBall = balls[0];
    // Calculate initial distance from first ball to paddle
    float closestDist = fabs(balls[0]->getPosition().x - mPosition.x);
    // Check remaining active balls
    for (int i = 1; i < activeBalls; i++) {
        float dist = fabs(balls[i]->getPosition().x - mPosition.x);
        if (dist < closestDist) {
            closestDist = dist;
            closestBall = balls[i];
        }
    }

    return closestBall;
}