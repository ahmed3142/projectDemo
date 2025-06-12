#include "game.h"
#include "textureloader.h"
#include "tower.h"
#include "level.h"
#include "projectile.h"

#define rep(i, n) for (int i = 0; i < (n); ++i)
#define rrep(i, a, b) for (int i = (a); i < (b); ++i)

Game::Game(int windowWidth, int windowHeight)
    : PlacementModeCurrent(PlacementMode::wall),
      level(windowWidth / tileSize, windowHeight / tileSize),
      spawnTimer(2.0f), roundTimer(5.0f)
{
    textureOverlay = *TextureLoader::LoadTextureFromFile("Overlay.png"); // menu

    const float deltaTime = 1.0f / 60.0f;
    float accumulator = 0.0f;

    bool running = true;
    SetTargetFPS(60);

    while (running && !WindowShouldClose())
    {
        float delta = GetFrameTime();
        accumulator += delta;

        while (accumulator >= deltaTime)
        {
            processEvents(running);
            update(deltaTime);
            accumulator -= deltaTime;
        }

        draw();
    }
}

Game::~Game()
{
    TextureLoader::DeallocTexture(); // Deallocate all textures
}

void Game::processEvents(bool &running)
{ // for every frame
    bool mouseDownThisFrame = (mouseDownStatus == 0);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        mouseDownThisFrame = true;
        mouseDownStatus = 1;
        // cout << mouseDownStatus << endl;
        // cout << "Mouse left button pressed" << endl;
    }
    else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        mouseDownThisFrame = true;
        mouseDownStatus = 2;
        // cout << mouseDownStatus << endl;
        // cout << "Mouse right button pressed" << endl;
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
    {
        mouseDownStatus = 0;
        // cout << "Mouse button released" << endl;
    }
    if (IsKeyPressed(KEY_ESCAPE))
        running = false;
    if (IsKeyPressed(KEY_ONE))
    {
        cout << "Placement mode: wall" << endl;
        PlacementModeCurrent = PlacementMode::wall;
    }
    if (IsKeyPressed(KEY_TWO))
    {
        cout << "Placement mode: tower" << endl;
        PlacementModeCurrent = PlacementMode::tower;
    }
    if (IsKeyPressed(KEY_M))
    {
        cout << "Overlay toggled" << endl;
        overlayVisible = !overlayVisible;
    }
    if (IsKeyPressed(KEY_SPACE))
    {
        newRound(); // Start a new round when SPACE is pressed
    }

    Vector2 mouse = GetMousePosition();
    Vector2 mousePosition = {mouse.x / tileSize, mouse.y / tileSize};

    if (mouseDownStatus > 0)
    {
        if (mouseDownStatus == 1)
        { // left mouse button
            if (PlacementModeCurrent == PlacementMode::wall)
            {
                level.setTileWall((int)mousePosition.x, (int)mousePosition.y, true);
            }
            else if (PlacementModeCurrent == PlacementMode::tower)
            {
                if (mouseDownThisFrame)
                    addTower(mousePosition);
            }
        }
        else if (mouseDownStatus == 2)
        { // right mouse button
            if (IsKeyDown(KEY_R))
                removeTower(mousePosition);
            else if (IsKeyPressed(KEY_U))
                upgradeTower(mousePosition);
        }
    }

    // tower range display
    hoveredTower = nullptr; // Reset hovered tower
    for (auto &tower : towers)
    {
        if (tower->checkIfOnTile((int)mousePosition.x, (int)mousePosition.y))
        {
            hoveredTower = tower; // Set hovered tower if mouse is over a tower
            break;
        }
    }
}

void Game::addUnit(Vector2 spawnPos, EnemyType type)
{
    units.push_back(make_shared<Unit>(spawnPos, type));
}

// void Game::removeUnit(Vector2 mousePosition){
//     rep(i,units.size()){
//         if(units[i].checkOverlap(mousePosition, 0.0f)){
//             units.erase(units.begin()+i);
//             i--;
//         }
//     }
// }

void Game::draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    level.draw(tileSize); // level

    for (auto &unit : units)
    { // enemy units
        if (unit != nullptr)
            unit->draw(tileSize);
    }

    for (auto &tower : towers)
    { // towers
        tower->draw(tileSize);
    }

    for (auto &projectile : projectiles)
    { // projectiles
        projectile.draw(tileSize);
    }

    if (overlayVisible)
        DrawTexture(textureOverlay, 40, 40, WHITE);

    if (roundCompleted)
    {
        int textposx = 1488 / 2 - 200;
        int textposy = 912 / 2 - 100;
        DrawText("Round completed! Press SPACE to start a new round.", textposx, textposy, 20, BLACK);
    }

    // tower range display
    if (hoveredTower != nullptr)
    {
        Vector2 center = {
            (hoveredTower->getPosition().x + 0.5f) * tileSize,
            (hoveredTower->getPosition().y + 0.5f) * tileSize};
        float pixelRadius = hoveredTower->getRange() * tileSize;
        DrawCircle((int)center.x, (int)center.y, pixelRadius, Fade(BLUE, 0.2f));
    }
    EndDrawing();
}

void Game::updateRoundSpawn(float deltaTime)
{
    if (!roundStarted)
        return; // Skip if round not started yet

    spawnTimer.countDown(deltaTime);

    if (units.empty() && spawnUnitCount == 0)
    {
        roundTimer.countDown(deltaTime);
        roundCompleted = true;
        if (roundTimer.timeSIsZero())
        {
            roundStarted = false; // Reset round state
        }
    }

    if (spawnUnitCount > 0 && spawnTimer.timeSIsZero())
    {
        EnemyType type = EnemyType::basic;

        if (spawnUnitCount % 20 == 0)
        {
            type = EnemyType::tank;
            enemySpawnCooldown = 3.0f;
        }
        else if (spawnUnitCount % 5 == 0)
        {
            type = EnemyType::basic;
            enemySpawnCooldown = 1.0f;
        }
        else
        {
            type = EnemyType::fast;
            enemySpawnCooldown = 0.2f;
        }

        currentEnemyType = type;
        addUnit(level.getRandomEnemySpawnerPosition(), type); // pass the type
        spawnUnitCount--;

        spawnTimer.setTo(enemySpawnCooldown);
    }
}

void Game::addTower(Vector2 mousePosition)
{
    Vector2 position = {(int)mousePosition.x, (int)mousePosition.y};
    if (level.isTileWall((int)position.x, (int)position.y))
    {
        for (auto &tower : towers)
        {
            if (tower->checkIfOnTile((int)position.x, (int)position.y))
            {
                // cout << "Tower already exists at position: (" << position.x << ", " << position.y << ")" << endl;
                return;
            }
        }
        TowerType type = IsKeyDown(KEY_S) ? TowerType::sniper : TowerType::basic;
        towers.push_back(make_shared<Tower>(position, type));

        // checking
        if (type == TowerType::sniper)
        {
            cout << "Sniper tower added at position: (" << position.x << ", " << position.y << ")" << endl;
        }
        else
        {
            cout << "Basic tower added at position: (" << position.x << ", " << position.y << ")" << endl;
        }
    }
}

void Game::removeTower(Vector2 mousePostion)
{
    level.setTileWall((int)mousePostion.x, (int)mousePostion.y, false);
    for (auto it = towers.begin(); it != towers.end();)
    {
        if ((*it)->checkIfOnTile((int)mousePostion.x, (int)mousePostion.y))
        {
            it = towers.erase(it);
            cout << "Tower removed at position: (" << mousePostion.x << ", " << mousePostion.y << ")" << endl;
        }
        else
        {
            ++it;
        }
    }
}

void Game::upgradeTower(Vector2 mousePosition)
{
    for (auto &tower : towers)
    {
        if (tower->checkIfOnTile((int)mousePosition.x, (int)mousePosition.y))
        {
            tower->upgrade();
            cout << "Tower upgraded!" << endl;
            break;
        }
    }
}

void Game::newRound()
{
    // Reset round state
    roundStarted = true;
    spawnUnitCount = 100;     // Reset spawn count
    roundTimer.resetToMax(); // Reset round timer
    roundCompleted = false;  // Reset round completed state
    // cout << "New round started!" << endl;
    level.printLevelInfo(); // Print level info for debugging
}

void Game::updateUnit(float deltaTime)
{
    auto it = units.begin();
    while (it != units.end())
    {
        if ((*it) != nullptr)
        {
            (*it)->update(deltaTime, level, units);
            if ((*it)->getIsAlive() == false)
            {
                it = units.erase(it);
                continue; // Remove dead unit and continue to next iteration
            }
        }
        ++it; // Move to the next unit
    }
}

void Game::updateProjectiles(float deltaTime)
{
    auto it = projectiles.begin();
    while (it != projectiles.end())
    {
        (*it).update(deltaTime, units);
        if ((*it).checkCollision())
        {
            it = projectiles.erase(it); // Remove projectile if it collided
        }
        else
            it++;
    }
}

void Game::update(float deltaTime)
{

    updateUnit(deltaTime); // update all units

    for (auto &tower : towers)
    { // tower update
        tower->update(deltaTime, units, projectiles);
    }

    updateProjectiles(deltaTime); // update all projectiles
    updateRoundSpawn(deltaTime);
}