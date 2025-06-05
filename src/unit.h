#pragma once

#include <raylib.h>
#include <vector>
#include "level.h"
#include <bits/stdc++.h>
#include <memory>

using namespace std;

class Unit {
    Vector2 position;
    static const float speed;
    static const float size;

    Texture2D texture;
    bool alive = true;

    const int healthBasic=2;
    int currentHealth = healthBasic;  

public: 
    Unit(Vector2 setPosition);
    void update(float deltaTime, Level& level, vector<shared_ptr<Unit>>& units);
    void draw(int tileSize);
    bool checkOverlap(Vector2 positionOfOtherUnit, float otherUnitSize);
    bool getIsAlive();
    Vector2 getPosition();
    int getCurrentHealth();
    void damage(int damageAmount);
};