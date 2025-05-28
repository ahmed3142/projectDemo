#pragma once
#include "level.h"
#include "unit.h"
#include "timer.h"
#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include "tower.h"

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

    void addUnit(Vector2 mousePostion);
    void removeUnit(Vector2 mousePosition);
    
    void addTower(Vector2 mousePosition);
    void removeTower(Vector2 mousePosition);

    Timer spawnTimer, // gap between spawning units
          roundTimer; // gap between rounds
    int spawnUnitCount=0,
        roundCount=0;

    int mouseDownStatus=0;

    const int tileSize= 48;
    Level level;
    
    vector<Unit> units;
    vector<Tower> towers;

    Texture2D textureOverlay;
    bool overlayVisible=false;
    


    public:
        Game(int windowWidth, int windowHeight);
        ~Game();
};