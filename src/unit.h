#pragma once

#include <raylib.h>
#include <vector>
#include "level.h"

class Unit {
    Vector2 position;
    static const float speed;
    static const float size;

    Texture2D texture;
    bool alive = true;

public: 
    Unit(Vector2 setPosition);
    void update(float deltaTime, Level& level, vector<Unit>& units);
    void draw(int tileSize);
    bool checkOverlap(Vector2 positionOfOtherUnit, float otherUnitSize);
    bool isAlive();
};