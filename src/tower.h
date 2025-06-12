#pragma once
#include <raylib.h>
#include <vector>
#include "textureloader.h"
#include <raymath.h>
#include <bits/stdc++.h>
#include <memory>
#include "unit.h"
#include "projectile.h"
#include "timer.h"
#include "projectile.h"

using namespace std;
enum class TowerType {
        basic,
        sniper 
};

class Tower {
    TowerType type;

    // int tileX;
    // int tileY;
    // int tileSize;
    // int health;
    // int damage;
    // static const float attackRange;
    // float attackCooldown;
    // float attackCooldownCurrent;

    float range;
    float fireCooldown;
    float projectileSpeed;
    float projectileMaxDistance;
    int projectileDamage;

    int towerLevel = 1;
    static const int maxTowerLevel = 3;


    Vector2 position;
    float angle;
    static const float angularSpeed; // Speed of rotation in radians per second
    
    Texture2D textureTileTower;

    weak_ptr<Unit> findEnemy(vector<shared_ptr<Unit>>& units);

    weak_ptr<Unit> targetEnemy;

    bool updateAngle(float deltaTime);
    void shoot(vector<Projectile>& projectiles);

    Timer weaponTimer; // Timer for weapon cooldown


    public:
        Tower(Vector2 setPosition, TowerType setType);
        
        void update(float deltaTime, vector<shared_ptr<Unit>>& units, vector<Projectile>& projectiles);
        void upgrade();
        void draw(int tileSize);
        Vector2 getPosition() const;
        float getRange() const;
        
        bool checkIfOnTile(int x, int y);
};