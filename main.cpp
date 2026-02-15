/**
 * Author: Ian Fiterman
 * Assignment: Simple 2D Scene
 * Date due: 02/14/2024
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

#include "CS3113/cs3113.h"
#include <array>
#include <map>
#include <math.h>
#include <stdlib.h>
#include <string>

// Global Constants
constexpr int SCREEN_WIDTH = 1600 / 2, SCREEN_HEIGHT = 900 / 2, FPS = 60,
              SIZE = 200, FRAME_LIMIT = 100;
constexpr float TWO_PI = 2 * PI, GROUND_OFFSET = 100.0f, EDGE_MARGIN = 150.0f;
constexpr char BG_COLOUR[] = "#000000";
constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
constexpr Vector2 BASE_SIZE = {(float)SIZE, (float)SIZE};

// Texture path constants
constexpr char LEBRON[] = "assets/lebron_sunshine.png";
constexpr char MAXWELL[] = "assets/maxwell_cat.png";
constexpr char SKIBIDI[] = "assets/skibidi_toilet.png";
constexpr char BALLERINA[] = "assets/ballerina_capuchina.png";

// Ballerina constants
constexpr float FIGURE_EIGHT_TIME = 6.0f, // Time to complete one figure 8 loop
    OSCILLATION_TIME = 2.0f,              // Time to oscillate back and forth
    X_RADIUS = 250.0f,                    // Horizontal radius of the figure 8
    Y_RADIUS = 75.0f,                     // Vertical radius of the figure 8
    OSCILLATION_AMPLITUDE = 20.0f;        // Max rotation angle (degrees)

// Maxwell constants
constexpr float ORBIT_TIME = 3.0f, // Time to complete one orbit
    FLIP_TIME = 1.0f,              // Time to complete one card flip rotation
    ORBIT_RADIUS = 150.0f;         // Distance from the center of Ballerina

// Skibidi constants
constexpr float RISE_TIME = 1.0f, // Time to rise up
    STAY_TIME = 1.0f,             // Time to stay visible
    FALL_TIME = 1.0f,             // Time to fall down
    HIDE_TIME = 1.0f,             // Time hidden underground
    POP_HEIGHT = 200.0f,          // How high Skibidi pops up
    BASE_WIDTH = 150.0f,          // Base width of Skibidi
    BASE_HEIGHT = 240.0f;         // Base height of Skibidi

// LeBron constants
constexpr float DROP_TIME = 1.0f, // Time to drop from top
    BOUNCE_TIME = 1.0f,           // Time for each bounce
    FALL_THROUGH_TIME = 1.0f,     // Time to fall through bottom
    PAUSE_TIME = 1.0f,            // Time hidden before reappearing
    MAX_HEIGHT = 350.0f,          // Initial bounce height
    HEIGHT_LOSS = 0.8f,           // Height multiplier per bounce
    SPIN_TIME = 2.0f,             // Time to complete one spin
    DROP_HEIGHT = 100.0f,         // Distance above screen to drop from
    CYCLE_TIME = DROP_TIME + (BOUNCE_TIME * 4) + FALL_THROUGH_TIME
               + PAUSE_TIME; // Total cycle time for LeBron

// Struct to represent texture object state and render method
struct TextureObject {
    TextureObject(const char* texturePath, Vector2 scale = BASE_SIZE,
                  Vector2 position = ORIGIN) :
        texturePath(texturePath), scale(scale), position(position) { }

    bool flipHorizontal = false; // Needed for Maxwell only

    void loadTexture() { texture = LoadTexture(texturePath); }

    void renderObject() {
        // Whole texture (UV coordinates)
        Rectangle textureArea = {0.0f, 0.0f,
                                 flipHorizontal ?
                                     -static_cast<float>(texture.width) :
                                     static_cast<float>(texture.width),
                                 static_cast<float>(texture.height)};

        // Destination rectangle – centered on texture position
        Rectangle destinationArea = {position.x, position.y,
                                     static_cast<float>(scale.x),
                                     static_cast<float>(scale.y)};

        // Origin inside the source texture (center of the texture)
        Vector2 originOffset = {static_cast<float>(scale.x) / 2.0f,
                                static_cast<float>(scale.y) / 2.0f};

        // Render the texture on screen
        DrawTexturePro(texture, textureArea, destinationArea, originOffset,
                       angle, WHITE);
    }

    const char* texturePath;
    Texture2D texture;
    Vector2 position;
    Vector2 scale;
    float angle = 0.0f;
};

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;
int gFrameCounter = 0;

// Global timers
float gLebronTime = 0.0f;
float gLebronSpinTime = 0.0f;
float gBallerinaTime = 0.0f;
float gBallerinaRotationTime = 0.0f;
float gMaxwellOrbitTime = 0.0f;
float gMaxwellRotationTime = 0.0f;
float gSkibidiTime = 0.0f;
float gRainbowTime = 0.0f;

// Global texture objects
TextureObject gLebronTexture(LEBRON, Vector2 {150.0f, 150.0f});
TextureObject gMaxwellTexture(MAXWELL, Vector2 {81.4f, 121.2f});
TextureObject gSkibidiTexture(SKIBIDI, Vector2 {150.0f, 240.0f});
TextureObject gBallerinaTexture(BALLERINA, Vector2 {180.0f, 200.0f});

// Function Declarations
void initialise();
void processInput();
void updateBallerina(float deltaTime);
void updateMaxwell(float deltaTime);
void updateSkibidi(float deltaTime);
void updateLebron(float deltaTime);
void update();
void render();
void shutdown();

// Function Definitions
void initialise() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 1 - Brainrot");

    gLebronTexture.loadTexture();
    gMaxwellTexture.loadTexture();
    gSkibidiTexture.loadTexture();
    gBallerinaTexture.loadTexture();

    SetTargetFPS(FPS);
}

void processInput() {
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void updateBallerina(float deltaTime) {
    // Update timers
    gBallerinaTime = fmod(gBallerinaTime + deltaTime, FIGURE_EIGHT_TIME);
    gBallerinaRotationTime =
        fmod(gBallerinaRotationTime + deltaTime, OSCILLATION_TIME);

    float figureEightPhase =
        (gBallerinaTime / FIGURE_EIGHT_TIME) * TWO_PI; // Normalize to 0-2pi
    float rotationPhase = (gBallerinaRotationTime / OSCILLATION_TIME)
                        * TWO_PI; // Normalize to 0-2pi

    // Figure 8 movement
    gBallerinaTexture.position.x = ORIGIN.x + X_RADIUS * cos(figureEightPhase);
    gBallerinaTexture.position.y =
        ORIGIN.y + Y_RADIUS * sin(figureEightPhase * 2.0f);
    // Oscillation back and forth
    gBallerinaTexture.angle = OSCILLATION_AMPLITUDE * sin(rotationPhase);
}

void updateMaxwell(float deltaTime) {
    // Update timers
    gMaxwellOrbitTime = fmod(gMaxwellOrbitTime + deltaTime, ORBIT_TIME);
    gMaxwellRotationTime = fmod(gMaxwellRotationTime + deltaTime, FLIP_TIME);

    float orbitPhase =
        (gMaxwellOrbitTime / ORBIT_TIME) * TWO_PI; // Normalize to 0-2pi

    // Orbit around the ballerina
    gMaxwellTexture.position.x =
        gBallerinaTexture.position.x + ORBIT_RADIUS * cos(orbitPhase);
    gMaxwellTexture.position.y =
        gBallerinaTexture.position.y + ORBIT_RADIUS * sin(orbitPhase);

    // Cardflip rotation
    float rotationPhase =
        (gMaxwellRotationTime / FLIP_TIME) * TWO_PI; // Normalize to 0-2pi
    float scaleX = fabs(cos(rotationPhase));
    gMaxwellTexture.scale.x = BASE_SIZE.x * scaleX;
    gMaxwellTexture.flipHorizontal = cos(rotationPhase) < 0;
}

void updateSkibidi(float deltaTime) {
    float cycleTime = RISE_TIME + STAY_TIME + FALL_TIME + HIDE_TIME;
    gSkibidiTime = fmod(gSkibidiTime + deltaTime, cycleTime);

    // Teleport once when entering hide phase
    static bool hasTeleported = false;
    float hiddenStart = RISE_TIME + STAY_TIME + FALL_TIME;

    if (gSkibidiTime > hiddenStart) {
        if (!hasTeleported) {
            gSkibidiTexture.position.x =
                EDGE_MARGIN
                + (rand() % (SCREEN_WIDTH - (int)(EDGE_MARGIN * 2)));
            hasTeleported = true;
        }
    } else {
        hasTeleported = false; // Reset flag when not in hide phase
    }

    // Pop animation with pause at top
    float popHeight;
    float scaleMultiplier;

    if (gSkibidiTime < RISE_TIME) {
        // Rising
        popHeight = POP_HEIGHT * (gSkibidiTime / RISE_TIME);
        scaleMultiplier = gSkibidiTime / RISE_TIME; // Grow from 0 to 1
    } else if (gSkibidiTime < RISE_TIME + STAY_TIME) {
        // Stay up
        popHeight = POP_HEIGHT;
        scaleMultiplier = 1.0f;
    } else if (gSkibidiTime < RISE_TIME + STAY_TIME + FALL_TIME) {
        // Falling
        float fallProgress =
            (gSkibidiTime - (RISE_TIME + STAY_TIME)) / FALL_TIME;
        popHeight = POP_HEIGHT * (1.0f - fallProgress);
        scaleMultiplier = 1.0f - fallProgress; // Shrink from 1 to 0
    } else {
        // Hide
        popHeight = 0.0f;
        scaleMultiplier = 0.0f;
    }

    gSkibidiTexture.scale = {BASE_WIDTH * scaleMultiplier,
                             BASE_HEIGHT * scaleMultiplier};
    gSkibidiTexture.position.y = SCREEN_HEIGHT + GROUND_OFFSET - popHeight;
}

void updateLebron(float deltaTime) {
    // Update timers
    gLebronTime = fmod(gLebronTime + deltaTime, CYCLE_TIME);
    gLebronSpinTime = fmod(gLebronSpinTime + deltaTime, SPIN_TIME);

    // Teleport to random X position during pause phase
    static bool hasRepositioned = false;
    float hideStart = DROP_TIME + (BOUNCE_TIME * 4) + FALL_THROUGH_TIME;

    if (gLebronTime > hideStart && gLebronTime < hideStart + PAUSE_TIME) {
        if (!hasRepositioned) {
            gLebronTexture.position.x =
                EDGE_MARGIN
                + (rand() % (SCREEN_WIDTH - (int)(EDGE_MARGIN * 2)));
            hasRepositioned = true;
        }
        // Keep LeBron offscreen during hide phase
        gLebronTexture.position.y = SCREEN_HEIGHT + DROP_HEIGHT;
    } else {
        hasRepositioned = false;
    }

    float groundLevel = SCREEN_HEIGHT - GROUND_OFFSET;

    if (gLebronTime < DROP_TIME) {
        // Falling from top
        float fallProgress = gLebronTime / DROP_TIME;
        gLebronTexture.position.y =
            -DROP_HEIGHT + (groundLevel + DROP_HEIGHT) * fallProgress;
    } else if (gLebronTime < DROP_TIME + BOUNCE_TIME * 4) {
        // Bouncing phase
        float bounceTime = gLebronTime - DROP_TIME;

        float maxHeight = MAX_HEIGHT;
        if (bounceTime > BOUNCE_TIME) maxHeight *= HEIGHT_LOSS;
        if (bounceTime > BOUNCE_TIME * 2) maxHeight *= HEIGHT_LOSS;
        if (bounceTime > BOUNCE_TIME * 3) maxHeight *= HEIGHT_LOSS;

        float localBounce = fmod(bounceTime, BOUNCE_TIME);
        float bounceHeight = maxHeight * fabs(sin(localBounce * PI));

        gLebronTexture.position.y = groundLevel - bounceHeight;
    } else if (gLebronTime < DROP_TIME + BOUNCE_TIME * 4 + FALL_THROUGH_TIME) {
        // Fall through bottom
        float fallProgress =
            (gLebronTime - (DROP_TIME + BOUNCE_TIME * 4)) / FALL_THROUGH_TIME;
        gLebronTexture.position.y =
            groundLevel + (SCREEN_HEIGHT + DROP_HEIGHT) * fallProgress;
    }

    // Spin continuously
    float rotationPhase = (gLebronSpinTime / SPIN_TIME) * TWO_PI;
    gLebronTexture.angle =
        (rotationPhase / TWO_PI) * 360.0f; // Convert to degrees
}

void update() {
    // Delta time
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    updateLebron(deltaTime);
    updateBallerina(deltaTime);
    updateMaxwell(deltaTime);
    updateSkibidi(deltaTime);
}

void render() {
    BeginDrawing();

    gRainbowTime += GetFrameTime();
    if (gRainbowTime > 6.0f) gRainbowTime -= 6.0f;

    // Cycle through 360 degrees of hue
    float hue = fmod(gRainbowTime * 60.0f, 360.0f);
    // Use permitted by professor to create rainbow cycle
    Color rainbowColor =
        ColorFromHSV(hue, 1.0f, 1.0f); // Full saturation and value

    ClearBackground(rainbowColor);

    // Render the texture on screen
    gLebronTexture.renderObject();
    gBallerinaTexture.renderObject();
    gMaxwellTexture.renderObject();
    gSkibidiTexture.renderObject();
    EndDrawing();
}

void shutdown() {
    CloseWindow();
    UnloadTexture(gLebronTexture.texture);
    UnloadTexture(gMaxwellTexture.texture);
    UnloadTexture(gSkibidiTexture.texture);
    UnloadTexture(gBallerinaTexture.texture);
}

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