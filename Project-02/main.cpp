/**
 * Author: Ian Fiterman
 * Assignment: Project 2: Pong Variations
 * Date due: 02/28/2024
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

#include "CS3113/Ball.h"
#include "CS3113/Constants.h"
#include "CS3113/Entity.h"
#include "CS3113/Paddle.h"

// Global Constants
constexpr char BG_COLOUR[] = "#000000";
constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};

const int SCORE_FONT_SIZE = 50, TEXT_FONT_SIZE = 30,
          LEFT_SCORE_X = SCREEN_WIDTH / 4,
          RIGHT_SCORE_X = SCREEN_WIDTH * 3 / 4 - 20, SCORE_Y = 25,
          CENTER_TEXT_Y = SCREEN_HEIGHT / 2 - 15;

// Player enum
enum Player { NONE, LEFT_P, RIGHT_P, BOTH };

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;

int gLeftScore = 0;
int gRightScore = 0;
bool gSinglePlayer = false;
bool gPaused = true;
bool gStarted = false;
int gActiveBalls = 1;
Player gWinner = NONE;

// Entities
Paddle* left_paddle = nullptr;
Paddle* right_paddle = nullptr;
std::vector<Ball*> gBalls;
Entity* gWinAnimation = nullptr;

// Function Declarations (game loop)
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Local Function Declarations
void setBallCount(int count);
void resetGame();
void renderAllText();
void renderScores(Player players);
void setWinAnimPos();

int main(void) {
    initialise();

    while (gAppStatus == RUNNING) {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}

void initialise() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "We Got Pong 67 Before GTA 6");
    // Set left paddle at left edge, vertically centred
    left_paddle = new Paddle(Vector2 {25.0f, SCREEN_HEIGHT / 2},
                             Vector2 {25.0f, 100.0f}, "assets/paddle.png");
    left_paddle->setFlipped(true); // Flip left paddle horizontally
    // Set right paddle at right edge, vertically centred
    right_paddle = new Paddle(Vector2 {SCREEN_WIDTH - 25.0f, SCREEN_HEIGHT / 2},
                              Vector2 {25.0f, 100.0f}, "assets/paddle.png");
    // Initialize balls in centre of screen with random movement direction
    setBallCount(1);
    // Initialize win animation entity (hidden until game over)
    gWinAnimation =
        new Entity(ORIGIN, Vector2 {100.0f, 100.0f}, "assets/win.png", ATLAS,
                   Vector2 {1, 10}, {{DOWN, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}}});
    gWinAnimation->setAlwaysAnimate(
        true); // Animate win animation even without movement
    gWinAnimation->setFrameSpeed(10);
    SetTargetFPS(FPS);
}

void processInput() {
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
    if (IsKeyPressed(KEY_P)) { // Pause/unpause game
        if (gWinner == NONE) {
            gPaused = !gPaused;
            if (!gStarted)
                gStarted = true; // Mark game as started on first unpause
            if (!gPaused)
                gPreviousTicks = (float)GetTime(); // Reset timer on unpause
        }
    }

    if (IsKeyPressed(KEY_R)) resetGame(); // Reset game state
    // Toggle single-player mode
    if (IsKeyPressed(KEY_T)) gSinglePlayer = !gSinglePlayer;
    // Ball count controls
    if (IsKeyPressed(KEY_ONE)) setBallCount(1);
    if (IsKeyPressed(KEY_TWO)) setBallCount(2);
    if (IsKeyPressed(KEY_THREE)) setBallCount(3);
    // Easter egg
    if (IsKeyDown(KEY_SIX) && IsKeyPressed(KEY_SEVEN)) setBallCount(67);
    // Left paddle controls always active
    if (IsKeyDown(KEY_W)) left_paddle->moveUp();
    if (IsKeyDown(KEY_S)) left_paddle->moveDown();
    // Right paddle only controllable in 2-player mode
    if (!gSinglePlayer) {
        if (IsKeyDown(KEY_UP)) right_paddle->moveUp();
        if (IsKeyDown(KEY_DOWN)) right_paddle->moveDown();
    }
}

void update() {
    // Delta time
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    // Check for winner
    int winScore = gActiveBalls == 67 ? 67 : 10;
    if (gLeftScore >= winScore && gWinner == NONE) {
        gWinner = LEFT_P;
    } else if (gRightScore >= winScore && gWinner == NONE) {
        gWinner = RIGHT_P;
    }
    if (gWinner != NONE) { // Someone won
        setWinAnimPos();
        gPaused = true;
        if (gActiveBalls == 67) {
            gWinAnimation->update(deltaTime); // Update only on game over
        }
    }

    if (gPaused) return; // Don't update game entities if paused
    if (gSinglePlayer) { // Update AI paddle movement if in single-player mode
        right_paddle->singlePlayerAI(gBalls, gActiveBalls);
    }
    // Update entities
    left_paddle->update(deltaTime);
    right_paddle->update(deltaTime);
    for (int i = 0; i < gActiveBalls; i++) {
        gBalls[i]->update(deltaTime, left_paddle, right_paddle, gLeftScore,
                          gRightScore);
    }
}

void render() {
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));
    // Render entities
    left_paddle->render();
    right_paddle->render();
    for (int i = 0; i < gActiveBalls; i++) {
        gBalls[i]->render();
    }
    renderAllText(); // Render text
    // Render win animation if game over in 67 mode
    if (gWinner != NONE && gActiveBalls == 67) { gWinAnimation->render(); }

    EndDrawing();
}

void shutdown() {
    delete left_paddle;
    delete right_paddle;
    delete gWinAnimation;
    for (Ball* ball : gBalls) {
        delete ball;
    }
    CloseWindow();
}

// Sets the number of active balls in the game, resetting all balls when changed
void setBallCount(int count) {
    // Allocate more balls if needed
    while (gBalls.size() < static_cast<size_t>(count)) {
        Ball* b = new Ball(ORIGIN, Vector2 {20.0f, 20.0f}, "assets/ball.png");
        gBalls.push_back(b);
    }
    // Set active ball count and reset all balls
    gActiveBalls = count;
    for (size_t i = 0; i < gActiveBalls; i++) {
        Ball* b = gBalls[i];
        if (gActiveBalls == 67) {
            b->setBaseSpeed(Ball::SLOW_SPEED);    // Slow down balls for 67 mode
        } else {
            b->setBaseSpeed(Ball::FAST_SPEED); // 1-3 balls use default speed
        }
        b->reset();
    }
}

// Resets game state and pauses
void resetGame() {
    gLeftScore = 0;
    gRightScore = 0;
    left_paddle->setPosition(Vector2 {25.0f, SCREEN_HEIGHT / 2});
    right_paddle->setPosition(
        Vector2 {SCREEN_WIDTH - 25.0f, SCREEN_HEIGHT / 2});
    setBallCount(1);
    gPreviousTicks = (float)GetTime();
    gSinglePlayer = false; // Start in 2 player mode
    gPaused = true;        // Start paused to allow player(s) to prepare
    gStarted = false;      // Mark game as unstarted
    gWinner = NONE;        // Clear winner to allow new game
}

void renderAllText() {
    // Render game over text and return early
    if (gWinner != NONE) {
        const char* winText = // Display winner in game over message
            gWinner == LEFT_P ? "Left Player Wins!" : "Right Player Wins!";
        int textWidth = MeasureText(winText, TEXT_FONT_SIZE);
        DrawText(winText, SCREEN_WIDTH / 2 - textWidth / 2, CENTER_TEXT_Y,
                 TEXT_FONT_SIZE, WHITE);
        // In 67 mode, gif replaces winner's score
        if (gWinner == LEFT_P && gActiveBalls == 67)
            renderScores(RIGHT_P); // Right player lost - render their score
        else if (gWinner == RIGHT_P && gActiveBalls == 67)
            renderScores(LEFT_P);  // Left player lost - render their score
        else {
            // Render scores normally if not in 67 mode
            renderScores(BOTH);
        }
        return;
    }
    // Render scores normally
    renderScores(BOTH);
    // Render pause text if paused
    if (gPaused && gWinner == NONE) {
        const char* pauseText = gStarted ? "PAUSED" : "Press P to Play";
        int textWidth = MeasureText(pauseText, TEXT_FONT_SIZE);
        DrawText(pauseText, SCREEN_WIDTH / 2 - textWidth / 2, CENTER_TEXT_Y,
                 TEXT_FONT_SIZE, WHITE);
    }
}

void renderScores(Player players) {
    if (players == LEFT_P || players == BOTH) {
        DrawText(TextFormat("%d", gLeftScore), LEFT_SCORE_X, SCORE_Y,
                 SCORE_FONT_SIZE, WHITE);
    }
    if (players == RIGHT_P || players == BOTH) {

        DrawText(TextFormat("%d", gRightScore), RIGHT_SCORE_X, SCORE_Y,
                 SCORE_FONT_SIZE, WHITE);
    }
}

void setWinAnimPos() {
    // Left player won
    if (gWinner == LEFT_P) {
        gWinAnimation->setPosition(
            {(float)LEFT_SCORE_X
                 + MeasureText(TextFormat("%d", gLeftScore), SCORE_FONT_SIZE)
                       / 2.0f,               // Horizontal align
             SCORE_Y + gWinAnimation->getScale().y / 2.0f
                 - SCORE_FONT_SIZE / 2.0f}); // Vertical align
    } else {                                 // Right player won
        gWinAnimation->setPosition(
            {(float)RIGHT_SCORE_X
                 + MeasureText(TextFormat("%d", gRightScore), SCORE_FONT_SIZE)
                       / 2.0f,               // Horizontal align
             SCORE_Y + gWinAnimation->getScale().y / 2.0f
                 - SCORE_FONT_SIZE / 2.0f}); // Vertical align
    }
}