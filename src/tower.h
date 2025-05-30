#pragma once
#include <raylib.h>
#include <vector>
#include "textureloader.h"
#include <raymath.h>
#include <bits/stdc++.h>
#include <memory>
#include "unit.h"

using namespace std;

class Tower {
    // int tileX;
    // int tileY;
    // int tileSize;
    // int health;
    // int damage;
    static const float attackRange;
    // float attackCooldown;
    // float attackCooldownCurrent;
    Vector2 position;
    float angle;
    static const float angularSpeed; // Speed of rotation in radians per second
    
    Texture2D textureTileTower;

    weak_ptr<Unit> findEnemy(vector<shared_ptr<Unit>>& units);

    weak_ptr<Unit> targetEnemy;
    


    public:
        Tower(Vector2 setPosition);
        void update(float deltaTime, vector<shared_ptr<Unit>>& units);
        void draw(int tileSize);
        
        bool checkIfOnTile(int x, int y);
};