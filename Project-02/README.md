# Intro:

In the latest edition of "Ian's brainrot series", comes "We Got Pong 67 Before GTA 6", which is in fact the full title of my game. TL;DR I implemented _Pong_ as described by the requirements, replacing the paddles for "hands" and the balls to well... balls that say 67 on them, but also added a special... twist.

### Controls: 
- Left player moves with `W` and `S`
- Right player moves with `up arrow` and `down arrow`
- Press `T` at any point to toggle between single player and dual player mode (right paddle is "AI" controlled in single player)
- Press `P` on game boot to start the game, and again at any time to pause the game
- Press `R` at any point to reset the game state to the initial state (useful after someone wins)
- Press `1`, `2`, or `3` to toggle the ball count at any point

- **IMPORTANT PLEASE DON'T MISS I WORKED REALLY HARD ON THIS** Press `6` and `7` together at any point to trigger "67 mode" ooohhhh

### Continuous collision detection:
I really challenged myself to develop a robust collision system that works in any game state. I used the principle of continuous collision detection, implemented using a simple swept AABB algorithm that employs the "slab" method. This took hours of meticulous googling and watching yt videos to figure out, but it works well in my testing and I'm really happy with it. The approach fully solves the issue of the ball overlapping the paddle or fully tunnelling through it.

TL;DR instead of calculating a "discrete" collision with each paddle, my approach first determines if there will be a collision at any point within the ball's movement during a given frame:
- If there is no collision... we just move the ball
- If the sweep collision function determines a collision happens during this frame,
the code will snap the ball to the exact position and moment in time within the frame that the collision occurs, resolves it, and lets the ball continue moving on its new trajectory for the remaining time.
- Additionally, if there is any residual overlap (or the collision happened at the start of the frame), the code pushes the ball out of the paddle along the "minimum translation vector"

All constructs from Raylib used that were not (explicitly) covered in class, but permitted by Prof. Romero Cruz or Eric:
- Text: DrawText(), FormatText(), MeasureText()
- GetRandomValue() which is just a rand() wrapper
- Macro constants: DEG2RAD (PI/180.0f)and EPSILON (0.000001f)