#pragma once
#include "level.h"
#include "unit.h"
#include "timer.h"
#include "tower.h"
#include "projectile.h"
#include "leveldata.h"
#include "leveldataio.h"
#include "levelEditor.h"

#include <raylib.h>
#include <raymath.h>
#include <bits/stdc++.h>
#include <vector>
#include <memory>

enum class GameUIState
{
    MainMenu,
    LevelSelect,
    Playing,
    Paused,
    Controls,
    GameOver,
    LevelEditor
};

class Game
{
    enum class PlacementMode
    {
        wall,
        tower,
    } PlacementModeCurrent; // variable

    GameUIState currentState = GameUIState::MainMenu;

    void processEvents(bool &running);
    //void processEventsLevelEditor();
    void update(float deltaTime);

    void updateRoundSpawn(float deltaTime); // update spawning of units
    bool roundStarted = false;

    void draw();
    void drawUI();

    void newRound(); // start a new round

    void addUnit(Vector2 mousePostion, EnemyType);
    // void removeUnit(Vector2 mousePosition);
    void updateUnit(float deltaTime);
    void updateProjectiles(float deltaTime);

    void addTower(Vector2 mousePosition);
    void removeTower(Vector2 mousePosition);
    void upgradeTower(Vector2 mousePosition);

    shared_ptr<Tower> hoveredTower = nullptr;

    Timer spawnTimer, // gap between spawning units
        roundTimer;   // gap between rounds
    int spawnUnitCount = 0,
        roundCount = 0;

    float enemySpawnCooldown = 2.0f;
    EnemyType currentEnemyType = EnemyType::basic;

    int mouseDownStatus = 0;

    const int tileSize = 48;
    Level level;

    vector<Projectile> projectiles; // projectiles fired by towers
    vector<shared_ptr<Unit>> units;
    vector<shared_ptr<Tower>> towers;

    Texture2D textureOverlay;
    bool overlayVisible = false;

    bool roundCompleted = false;

    Rectangle startBtn = {600, 300, 300, 60};
    Rectangle levelSelectBtn = {600, 380, 300, 60};
    Rectangle controlsBtn = {600, 460, 300, 60};
    Rectangle quitBtn = {600, 540, 300, 60};
    Rectangle resumeBtn = {600, 380, 300, 60};
    Rectangle backBtn = {50, 50, 120, 40};
    Rectangle levelEditorBtn = {600, 620, 300, 60};


    Rectangle gameOverMainMenuBtn = {600, 300, 300, 60};
    Rectangle gameOverLevelSelectBtn = {600, 380, 300, 60};
    Rectangle gameOverRestartBtn = {600, 460, 300, 60};
    Rectangle instantGameOverBtn   = {1280,  852, 150, 40};

    int selectLevelIndex = -1;
    vector<LevelData> allLevels;

    // money
    int baseMoney = 1000;
    int money = 0;
    int baseIncome = 500;
    int incomeIncrement = 100;

    int calculateUpgradeCost(shared_ptr<Tower> t);

    // selling tower
    bool sellConfirm = false;
    Timer sellConfirmTimer = Timer(2.0f);
    bool showMoneyWarning = false;
    Timer moneyWarningTimer = Timer(2.0f);

    shared_ptr<Tower> selectedTower = nullptr;

    // double click
    Timer doubleClickTimer = Timer(0.4f); // max time for double click
    Vector2 lastClickedTile = {-1, -1};
    void towerSelectionAndDoubleClickUpdate(Vector2 mouse);
    void selectedTowerDisplay();

    // single click lock
    Timer clickLockTimer = Timer(0.1f);

    // castle(target tile) health
    int targetHealth = 100;

    // game over
    bool gameOver = false;

    int spawnFastCount = 0;
    int spawnBasicCount = 0;
    int spawnTankCount = 0;

    // randomize spawnQueue
    deque<EnemyType> spawnQueue;

    // round calc
    const int maxRounds = 3;
    bool gameWon = false;

    // level editor
    unique_ptr<LevelEditor> levelEditor;

public:
    Game(int windowWidth, int windowHeight, const LevelData &data);
    ~Game();
};