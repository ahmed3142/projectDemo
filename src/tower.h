#pragma once
#include <raylib.h>
#include <vector>
#include "textureloader.h"
#include <raymath.h>
#include <iostream>

using namespace std;

class Tower {
    // int tileX;
    // int tileY;
    // int tileSize;
    // int health;
    // int damage;
    // float attackRange;
    // float attackCooldown;
    // float attackCooldownCurrent;
    Vector2 position;
    float angle;
    static const float angularSpeed; // Speed of rotation in radians per second
    
    Texture2D textureTileTower;


    public:
        Tower(Vector2 setPosition);
        void update(float deltaTime);
        void draw(int tileSize);
        
        bool checkIfOnTile(int x, int y);
};