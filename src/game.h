#pragma once
#include "level.h"
#include "unit.h"
#include "timer.h"
#include "tower.h"
#include "projectile.h"

#include <raylib.h>
#include <raymath.h>
#include <bits/stdc++.h>
#include <vector>
#include <memory>

using namespace std;


class Game {
    enum class PlacementMode{
        wall,
        tower,
    } PlacementModeCurrent ; // variable

    void processEvents(bool& running);
    void update(float deltaTime);


    void updateRoundSpawn(float deltaTime); // update spawning of units  
    bool roundStarted =false;

    void draw();

    void newRound(); // start a new round

    void addUnit(Vector2 mousePostion, EnemyType);
    //void removeUnit(Vector2 mousePosition);
    void updateUnit(float deltaTime);
    void updateProjectiles(float deltaTime);
    
    void addTower(Vector2 mousePosition);
    void removeTower(Vector2 mousePosition);
    void upgradeTower(Vector2 mousePosition);

    shared_ptr<Tower>  hoveredTower =nullptr;

    Timer spawnTimer, // gap between spawning units
          roundTimer; // gap between rounds
    int spawnUnitCount=0,
        roundCount=0;

    float enemySpawnCooldown = 2.0f;
    EnemyType currentEnemyType = EnemyType::basic;

    int mouseDownStatus=0;

    const int tileSize= 48;
    Level level;
    
    vector<Projectile> projectiles; // projectiles fired by towers
    vector<shared_ptr<Unit>> units;
    vector<shared_ptr<Tower>> towers;

    Texture2D textureOverlay;
    bool overlayVisible=true;
    
    bool roundCompleted = false;

    public:
        Game(int windowWidth, int windowHeight);
        ~Game();
};