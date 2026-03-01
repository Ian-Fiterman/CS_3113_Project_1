#ifndef CS3113_H
#define CS3113_H
#define LOG(argument) std::cout << argument << '\n'

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <cfloat>
#include <map>
#include <math.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <vector>

enum AppStatus { TERMINATED, RUNNING };

enum TextureType { SINGLE, ATLAS };

Color ColorFromHex(const char* hex);
void Normalise(Vector2* vector);
float GetLength(const Vector2 vector);
Rectangle getUVRectangle(const Texture2D* texture, int index, int rows,
                         int cols);

// Added this dupe of std::clamp() which was added in C++17
template <typename T>
T clamp(T val, T mn, T mx) {
    return std::min(std::max(val, mn), mx);
}

#endif // CS3113_H