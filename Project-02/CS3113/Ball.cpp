#include "Ball.h"
#include "Paddle.h"

/**
 * @brief Updates the ball's position, handles swept collision then
 *        depenetration as a safety net, screen edge bounce, and scoring
 */
void Ball::update(float deltaTime, Paddle* leftPaddle, Paddle* rightPaddle,
                  int& leftScore, int& rightScore) {
    // Calculate swept collision normal vectors
    Vector2 normalLeft, normalRight;
    float tLeft = sweepCollision(leftPaddle, deltaTime, normalLeft);
    float tRight = sweepCollision(rightPaddle, deltaTime, normalRight);
    // Determine which paddle hit first, if any
    Paddle* sweptPaddle = nullptr;
    Vector2 contactNormal = {0.0f, 0.0f};
    float tImpact = -1.0f;
    if (tLeft >= 0.0f
        && (tRight < 0.0f || tLeft <= tRight)) { // Left paddle hit first
        sweptPaddle = leftPaddle;
        tImpact = tLeft;
        contactNormal = normalLeft;
    } else if (tRight >= 0.0f) { // Right paddle hit first
        sweptPaddle = rightPaddle;
        tImpact = tRight;
        contactNormal = normalRight;
    }
    // If any paddle is hit
    if (sweptPaddle) {
        mPosition.x +=
            mMovement.x * mSpeed * deltaTime * tImpact; // Move to contact point
        mPosition.y +=
            mMovement.y * mSpeed * deltaTime * tImpact; // Move to contact point
        resolveCollision(sweptPaddle,
                         contactNormal); // Resolve collision at contact point
        float remaining =
            1.0f - tImpact; // Continue moving for remainder of frame
        mPosition.x += mMovement.x * mSpeed * deltaTime * remaining;
        mPosition.y += mMovement.y * mSpeed * deltaTime * remaining;
    } else {
        Entity::update(deltaTime); // No collision: move normally
    }
    // Screen edge bounce
    if (mPosition.y - mRadius < 0) {
        mPosition.y = mRadius;
        mMovement.y = -mMovement.y;
    } else if (mPosition.y + mRadius > SCREEN_HEIGHT) {
        mPosition.y = SCREEN_HEIGHT - mRadius;
        mMovement.y = -mMovement.y;
    }
    // Depenetrate paddle (if paddle passes over the ball)
    depenetrate(leftPaddle);
    depenetrate(rightPaddle);
    // Scoring
    if (mPosition.x - mRadius > SCREEN_WIDTH) {
        leftScore++;
        reset();
    } else if (mPosition.x + mRadius < 0) {
        rightScore++;
        reset();
    }
}

/**
 * @brief Performs a sweep collision check using the slab method
 * @param paddle
 * @param deltaTime
 * @param outNormal normal vector of impact
 * @return the time of impact with the paddle
 */
float Ball::sweepCollision(const Paddle* paddle, float deltaTime,
                           Vector2& outNormal) const {
    // Get paddle position and collider
    Vector2 paddlePos = paddle->getPosition();
    Vector2 paddleCol = paddle->getColliderDimensions();
    // Calculate paddle collider bounds expanded by ball radius
    float rectLeft = paddlePos.x - paddleCol.x / 2.0f - mRadius;
    float rectRight = paddlePos.x + paddleCol.x / 2.0f + mRadius;
    float rectTop = paddlePos.y - paddleCol.y / 2.0f - mRadius;
    float rectBottom = paddlePos.y + paddleCol.y / 2.0f + mRadius;
    Vector2 paddleMovement = paddle->getMovement();
    // Compute paddle velocity
    Vector2 paddleVel = {paddleMovement.x * paddle->getSpeed() * deltaTime,
                         paddleMovement.y * paddle->getSpeed() * deltaTime};
    // Compute relative velocity of the ball with respect to the paddle
    Vector2 relVel = {mMovement.x * mSpeed * deltaTime - paddleVel.x,
                      mMovement.y * mSpeed * deltaTime - paddleVel.y};
    // Epsilon is from raymath and its 0.00001f to prevent floating point errs
    if (fabsf(relVel.x) < EPSILON && fabsf(relVel.y) < EPSILON) return -1.0f;
    // Initialize entry and exit times for "slab test"
    // Note: This took a crap ton of googling, reading, and trial and error to
    // figure out
    float tEntryX = -INFINITY, tExitX = INFINITY, tEntryY = -INFINITY,
          tExitY = INFINITY;
    if (fabsf(relVel.x) < EPSILON) { // Near 0 horizontal velocity
        if (mPosition.x < rectLeft || mPosition.x > rectRight) return -1.0f;
    }
    if (fabsf(relVel.y) < EPSILON) { // Near 0 vertical velocity
        if (mPosition.y < rectTop || mPosition.y > rectBottom) return -1.0f;
    }
    // Compute horizontal entry and exit times
    tEntryX = (rectLeft - mPosition.x) / relVel.x;
    tExitX = (rectRight - mPosition.x) / relVel.x;
    if (tEntryX > tExitX) std::swap(tEntryX, tExitX);
    // Compute vertical entry and exit times
    tEntryY = (rectTop - mPosition.y) / relVel.y;
    tExitY = (rectBottom - mPosition.y) / relVel.y;
    if (tEntryY > tExitY) std::swap(tEntryY, tExitY);
    // Compute overall entry and exit times
    float tEntry = std::max(tEntryX, tEntryY);
    float tExit = std::min(tExitX, tExitY);
    // If no collision this frame, return -1
    if (tEntry > tExit || // No collision at all (ignore)
        tExit < 0.0f ||   // Collision in the past (ignore)
        tEntry > 1.0f)    // Collision in the future (ignore)
        return -1.0f;
    // Determine collision normal based on which axis we hit first
    if (tEntryY > tEntryX) // Hit horizontal face, normal is vertical
        outNormal = {0.0f, relVel.y > 0 ? -1.0f : 1.0f};
    else                   // Hit vertical face, normal is horizontal
        outNormal = {relVel.x > 0 ? -1.0f : 1.0f, 0.0f};
    if (tEntry < 0.0f && tExit > 0.0f)
        tEntry = 0.0f; // Clamp to 0 if collision at start of frame
    return tEntry;     // Return time of impact
}

/**
 * @brief Resolves bounce direction and speed after a collision. Assumes
 * position corrected in update()
 * @param paddle
 * @param normal
 */
void Ball::resolveCollision(Paddle* const paddle, Vector2 normal) {
    if (fabsf(normal.y) > 0.5f) {
        // Top or bottom face: reflect vertical movement
        mMovement.y = -mMovement.y;
    } else {
        // Side face: reflect horizontally and apply hit offset
        float paddleHalfHeight = paddle->getScale().y / 2.0f;
        float hitOffset =
            clamp((mPosition.y - paddle->getPosition().y) / paddleHalfHeight,
                  -1.0f, 1.0f);
        mMovement.y = hitOffset;
        Normalise(&mMovement);
    }
    // Force horizontal direction based on paddle center
    bool isLeftPaddle = paddle->getPosition().x < SCREEN_WIDTH / 2.0f;
    bool behindPaddle = isLeftPaddle ? mPosition.x < paddle->getPosition().x :
                                       mPosition.x > paddle->getPosition().x;
    mMovement.x = behindPaddle ? (isLeftPaddle ? -1.0f : 1.0f) :
                                 (isLeftPaddle ? 1.0f : -1.0f);
    // If new collision, speed up
    if (paddle != lastCollision) {
        mSpeedMultiplier += 0.1f;
        lastCollision = paddle;
    }
    mSpeed = mBaseSpeed * mSpeedMultiplier;
}

/**
 * @brief Corrects residual overlap between paddle and the ball. Does not count
 * as a collision
 * @param paddle
 */
void Ball::depenetrate(const Paddle* paddle) {
    // Get paddle position and collider
    Vector2 paddlePos = paddle->getPosition();
    Vector2 paddleCol = paddle->getColliderDimensions();
    // Calculate paddle collider bounds
    float rectLeft = paddlePos.x - paddleCol.x / 2.0f;
    float rectRight = paddlePos.x + paddleCol.x / 2.0f;
    float rectTop = paddlePos.y - paddleCol.y / 2.0f;
    float rectBottom = paddlePos.y + paddleCol.y / 2.0f;
    // Find closest point on paddle bounds to ball center
    float pointX = clamp(mPosition.x, rectLeft, rectRight);
    float pointY = clamp(mPosition.y, rectTop, rectBottom);
    // Check if closest point is inside ball radius
    float distX = mPosition.x - pointX;
    float distY = mPosition.y - pointY;
    float distSq = distX * distX + distY * distY;
    // Case 1: No overlap at all: return
    if (distSq >= mRadius * mRadius) return;
    // Cas 2: Ball center inside paddle bounds: push out along shallowest axis
    if (distSq == 0.0f) {
        // Calculate overlap on each side
        float overlapLeft = mPosition.x - rectLeft;
        float overlapRight = rectRight - mPosition.x;
        float overlapTop = mPosition.y - rectTop;
        float overlapBottom = rectBottom - mPosition.y;
        // Find minimum overlap: calculate minimum translation vector (mtv)
        // RIP Music Television 24/7 music channels
        float minOverlap = overlapLeft;  // left
        Vector2 mtv = {-(minOverlap + mRadius), 0.0f};
        if (overlapRight < minOverlap) { // right
            minOverlap = overlapRight;
            mtv = {minOverlap + mRadius, 0.0f};
        }
        if (overlapTop < minOverlap) { // up
            minOverlap = overlapTop;
            mtv = {0.0f, -(minOverlap + mRadius)};
        }
        if (overlapBottom < minOverlap) { // down
            mtv = {0.0f, overlapBottom + mRadius};
        }
        // Adjust position by mtv
        mPosition.x += mtv.x;
        mPosition.y += mtv.y;
    } else {               // Center outside paddle but still overlapping
        float dist =
            sqrtf(distSq); // distance from ball center to closest point
        float penetration = mRadius - dist; // how much to push ball out
        mPosition.x += (distX / dist) * penetration;
        mPosition.y += (distY / dist) * penetration;
    }
}

/**
 * @brief Resets the ball's position and speed; randomizes movement
 */
void Ball::reset() {
    mPosition = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    mSpeedMultiplier = 1.0f;
    mSpeed = mBaseSpeed;
    lastCollision = nullptr;
    // Calculate random angle and convert to movement vector
    float angle = GetRandomValue(-45, 45) * DEG2RAD;
    float dirX = GetRandomValue(0, 1) ? 1.0f : -1.0f;
    mMovement = {dirX * cosf(angle), sinf(angle)};
}