#include "game.h"
#include "textureloader.h"
#include "tower.h"
#include "level.h"
#include "projectile.h"

using namespace std;

#define rep(i, n) for (int i = 0; i < (n); ++i)
#define rrep(i, a, b) for (int i = (a); i < (b); ++i)

Game::Game(int windowWidth, int windowHeight, const LevelData &data)
    : PlacementModeCurrent(PlacementMode::wall),
      level(windowWidth / tileSize, windowHeight / tileSize),
      spawnTimer(2.0f), roundTimer(5.0f),
      money(10000)
{
    textureOverlay = *TextureLoader::LoadTextureFromFile("Overlay.png"); // menu

    allLevels = loadAllLevelsFromFile("all_levels.json");
    if (allLevels.empty())
    {
        cout << "No levels found." << endl;
        exit(1);
    }

    const float deltaTime = 1.0f / 60.0f;
    float accumulator = 0.0f;

    bool running = true;
    SetTargetFPS(60);

    // level.loadFromData(data); // external level

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
    bool mouseClick = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    Vector2 mouse = GetMousePosition();

    if (currentState == GameUIState::MainMenu)
    {
        if (mouseClick && CheckCollisionPointRec(mouse, startBtn))
        {
            currentState = GameUIState::Playing;
        }
        else if (mouseClick && CheckCollisionPointRec(mouse, levelSelectBtn))
        {
            currentState = GameUIState::LevelSelect;
        }
        else if (mouseClick && CheckCollisionPointRec(mouse, controlsBtn))
        {
            currentState = GameUIState::Controls;
        }
        else if (mouseClick && CheckCollisionPointRec(mouse, quitBtn))
        {
            running = false;
        }
        return; // skip other input
    }

    // CONTROLS or LEVELSELECT BACK BUTTON
    if ((currentState == GameUIState::Controls || currentState == GameUIState::LevelSelect) && mouseClick && CheckCollisionPointRec(mouse, backBtn))
    {
        currentState = GameUIState::MainMenu;
        return;
    }

    if (currentState == GameUIState::LevelSelect)
    {
        for (int i = 0; i < (int)allLevels.size(); ++i)
        {
            Rectangle levelBtn = {600, 200 + i * 60, 300, 50};
            if (mouseClick && CheckCollisionPointRec(mouse, levelBtn))
            {
                // selectedLevelIndex = i;
                level.loadFromData(allLevels[i]);
                currentState = GameUIState::Playing;
                return;
            }
        }
    }

    // paused
    if (currentState == GameUIState::Paused)
    {
        if ((mouseClick && CheckCollisionPointRec(mouse, resumeBtn)) || IsKeyPressed(KEY_P))
        {
            currentState = GameUIState::Playing;
        }
        return;
    }

    // PLAYING
    if (currentState == GameUIState::Playing && IsKeyPressed(KEY_P))
    {
        currentState = GameUIState::Paused;
    }

    if (currentState != GameUIState::Playing)
        return;

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
    if (IsKeyPressed(KEY_S))
    {
        // selling tower

        if (selectedTower != nullptr)
        {
            sellConfirm = true;
            sellConfirmTimer.resetToMax();
        }
    }
    if (IsKeyPressed(KEY_SPACE))
    {
        newRound(); // Start a new round when SPACE is pressed
    }

    Vector2 mousePosition = {mouse.x / tileSize, mouse.y / tileSize};

    if (mouseDownStatus > 0)
    {
        if (mouseDownStatus == 1)
        { // left mouse button
            if (!clickLockTimer.timeSIsZero())
                return;

            if (PlacementModeCurrent == PlacementMode::wall)
            {
                level.setTileWall((int)mousePosition.x, (int)mousePosition.y, true);
                clickLockTimer.resetToMax();
            }
            else if (PlacementModeCurrent == PlacementMode::tower)
            {
                if (mouseDownThisFrame)
                    addTower(mousePosition), clickLockTimer.resetToMax();
            }

            // tower selection and double click update
            towerSelectionAndDoubleClickUpdate(mouse);
        }
        else if (mouseDownStatus == 2)
        { // right mouse button
            if (IsKeyDown(KEY_R))
                removeTower(mousePosition);
            else if (IsKeyPressed(KEY_U))
                upgradeTower(mousePosition);
        }
    }

    // hover tower
    hoveredTower = nullptr; // Reset hovered tower
    for (auto &tower : towers)
    {
        if (tower->checkIfOnTile((int)mousePosition.x, (int)mousePosition.y))
        {
            hoveredTower = tower; // Set hovered tower if mouse is over a tower
            break;
        }
    }
    if (sellConfirm && selectedTower != nullptr && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        removeTower(selectedTower->getPosition());
        sellConfirm = false;
        selectedTower = nullptr;
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

    // Gameplay
    if (currentState == GameUIState::Playing || currentState == GameUIState::Paused)
    {
        level.draw(tileSize);

        for (auto &unit : units)
        {
            if (unit)
                unit->draw(tileSize);
        }

        for (auto &tower : towers)
        {
            tower->draw(tileSize);
        }

        for (auto &projectile : projectiles)
        {
            projectile.draw(tileSize);
        }

        // Overlay (press M)
        if (overlayVisible)
        {
            DrawTexture(textureOverlay, 40, 40, WHITE);
        }

        // Round completion message
        if (roundCompleted)
        {
            int textX = 1488 / 2 - 200;
            int textY = 912 / 2 - 100;
            DrawText("Round completed! Press SPACE to start a new round.", textX, textY, 20, BLACK);
        }

        // Hovered tower range display
        if (hoveredTower != nullptr)
        {
            Vector2 center = {
                (hoveredTower->getPosition().x + 0.5f) * tileSize,
                (hoveredTower->getPosition().y + 0.5f) * tileSize};
            float pixelRadius = hoveredTower->getRange() * tileSize;

            DrawCircle((int)center.x, (int)center.y, pixelRadius, Fade(BLUE, 0.1f));
            DrawCircleLines((int)center.x, (int)center.y, pixelRadius, BLUE);
        }

        // selected tower display
        if (selectedTower != nullptr)
        {
            selectedTowerDisplay();
        }

        // drawing money
        DrawText(("Money: " + std::to_string(money)).c_str(), 30, 20, 30, BLACK);

        // drawing health
        DrawText(("Target Health: " + std::to_string(targetHealth)).c_str(), 30, 60, 30, RED);

        // gameover UI
        if (gameOver)
        {
            DrawText("GAME OVER!", 550, 450, 60, RED);
        }
    }

    // for other states
    drawUI();

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

        int towerCost = 0;

        switch (type)
        {
        case TowerType::sniper:
            towerCost = 400;
            break;

        case TowerType::basic:
            towerCost = 200;
            break;
        }

        bool towerAdded = false;

        if (money >= towerCost)
        {
            auto newTower = make_shared<Tower>(position, type);
            newTower->addSpentCost(towerCost);
            towers.push_back(newTower);
            money -= towerCost;
            towerAdded = true;
        }

        else
        {
            cout << "Not enough money" << endl;
        }

        // checking
        if (towerAdded)
        {
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
}

void Game::removeTower(Vector2 mousePostion)
{
    // level.setTileWall((int)mousePostion.x, (int)mousePostion.y, false);
    for (auto it = towers.begin(); it != towers.end();)
    {
        if ((*it)->checkIfOnTile((int)mousePostion.x, (int)mousePostion.y))
        {
            // it = towers.erase(it);
            // cout << "Tower removed at position: (" << mousePostion.x << ", " << mousePostion.y << ")" << endl;

            int spent = (*it)->getTotalSpent();
            int refund = 0.6 * spent;
            money += refund;

            it = towers.erase(it);

            cout << "Tower sold. Refunded: " << refund << " | Money: " << money << endl;
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
            // tower->upgrade();
            // cout << "Tower upgraded!" << endl;
            // break;
            if (tower->isMaxLevel())
            {
                return; // returns if max level (3)
            }

            int upgradeCost = calculateUpgradeCost(tower);

            bool towerUpgraded = false;

            if (money >= upgradeCost)
            {
                tower->upgrade();
                money -= upgradeCost;
                tower->addSpentCost(upgradeCost);
                towerUpgraded = true;
                cout << "Tower Upgraded! Money Left: " << money << endl;
            }
            else
            {
                cout << "Not enough money to upgrade" << endl;
            }

            break;
        }
    }
}

void Game::newRound()
{
    // Reset round state
    roundStarted = true;
    spawnUnitCount = 100;    // Reset spawn count
    roundTimer.resetToMax(); // Reset round timer
    roundCompleted = false;  // Reset round completed state
    // cout << "New round started!" << endl;
    // level.printLevelInfo(); // Print level info for debugging'

    money += baseIncome;
    baseIncome += incomeIncrement;
    roundCount++;
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
                Vector2 unitPosition = (*it)->getPosition();
                Vector2 targetPosition = level.getTargetPosition();

                if (Vector2Distance(unitPosition, targetPosition) < 1.5f)
                {
                    targetHealth -= 1; // reduce health
                    if (targetHealth <= 0)
                    {
                        targetHealth = 0;
                        gameOver = true;
                        cout << "GAME OVER! Target health reached 0." << endl;
                    }
                }
                else
                {
                    switch ((*it)->getEnemyType())
                    {
                    case EnemyType::basic:
                        money += 30;
                        break;
                    case EnemyType::fast:
                        money += 20;
                        break;
                    case EnemyType::tank:
                        money += 80;
                        break;
                    }
                }

                it = units.erase(it);
                continue;
            }
        }
        ++it;
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
    if (currentState == GameUIState::Paused || gameOver)
        return;

    updateUnit(deltaTime); // update all units

    for (auto &tower : towers)
    { // tower update
        tower->update(deltaTime, units, projectiles);
    }

    updateProjectiles(deltaTime); // update all projectiles
    updateRoundSpawn(deltaTime);

    // update sell confirm
    if (sellConfirm)
    {
        sellConfirmTimer.countDown(deltaTime);
        if (sellConfirmTimer.timeSIsZero())
        {
            sellConfirm = false;
            selectedTower = nullptr;
        }
    }

    // update click lock timer
    clickLockTimer.countDown(deltaTime);

    // update double click timer
    doubleClickTimer.countDown(deltaTime);
}

void Game::drawUI()
{
    if (currentState == GameUIState::MainMenu)
    {
        DrawText("TOWER DEFENSE", 580, 180, 40, DARKGRAY);
        DrawRectangleRec(startBtn, LIGHTGRAY);
        DrawText("Start Game", startBtn.x + 20, startBtn.y + 15, 20, BLACK);

        DrawRectangleRec(levelSelectBtn, LIGHTGRAY);
        DrawText("Select Level", levelSelectBtn.x + 20, levelSelectBtn.y + 15, 20, BLACK);

        DrawRectangleRec(controlsBtn, LIGHTGRAY);
        DrawText("Controls", controlsBtn.x + 20, controlsBtn.y + 15, 20, BLACK);

        DrawRectangleRec(quitBtn, LIGHTGRAY);
        DrawText("Quit", quitBtn.x + 20, quitBtn.y + 15, 20, BLACK);
    }
    else if (currentState == GameUIState::Controls)
    {
        DrawText("Controls:", 100, 100, 30, DARKGRAY);
        DrawText("- [1] Place Wall", 120, 150, 20, BLACK);
        DrawText("- [2] Place Tower", 120, 180, 20, BLACK);
        DrawText("- [R] Remove Tower", 120, 210, 20, BLACK);
        DrawText("- [U] Upgrade Tower", 120, 240, 20, BLACK);
        DrawText("- [M] Toggle Overlay", 120, 270, 20, BLACK);
        DrawText("- [SPACE] New Round", 120, 300, 20, BLACK);
        DrawRectangleRec(backBtn, LIGHTGRAY);
        DrawText("Back", backBtn.x + 10, backBtn.y + 10, 20, BLACK);
    }
    else if (currentState == GameUIState::Paused)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.2f));
        DrawText("Game Paused", 620, 250, 30, DARKGRAY);
        DrawRectangleRec(resumeBtn, LIGHTGRAY);
        DrawText("Resume", resumeBtn.x + 20, resumeBtn.y + 15, 20, BLACK);
    }
    else if (currentState == GameUIState::LevelSelect)
    {
        DrawText("Select Level", 100, 80, 40, DARKGRAY);

        for (int i = 0; i < (int)allLevels.size(); ++i)
        {
            Rectangle levelBtn = {600, 200 + i * 60, 300, 50};
            DrawRectangleRec(levelBtn, LIGHTGRAY);
            string label = "Level " + to_string(i + 1);
            DrawText(label.c_str(), levelBtn.x + 20, levelBtn.y + 15, 20, BLACK);
        }

        DrawRectangleRec(backBtn, LIGHTGRAY);
        DrawText("Back", backBtn.x + 10, backBtn.y + 10, 20, BLACK);
    }
}

int Game::calculateUpgradeCost(shared_ptr<Tower> t)
{

    int upgradeCost = 0;

    TowerType type = t->getTowerType();

    switch (type)
    {
    case TowerType::basic:
        upgradeCost = 100 + 50 * (t->getTowerLevel() - 1);
        break;
    case TowerType::sniper:
        int towerLevel = t->getTowerLevel();
        switch (towerLevel)
        {
        case 1:
            upgradeCost = 1000;
            break;
        case 2:
            upgradeCost = 500;
            break;
        }
        break;
    }

    return upgradeCost;
}

void Game::towerSelectionAndDoubleClickUpdate(Vector2 mouse)
{
    // tower selection
    Vector2 tilePos = {mouse.x / tileSize, mouse.y / tileSize};
    bool foundTower = false;

    for (auto &tower : towers)
    {
        if (tower->checkIfOnTile((int)tilePos.x, (int)tilePos.y))
        {
            selectedTower = tower;
            clickLockTimer.resetToMax();
            Vector2 clickedTile = {(int)tilePos.x, (int)tilePos.y};
            if (clickedTile.x == lastClickedTile.x && clickedTile.y == lastClickedTile.y &&
                !doubleClickTimer.timeSIsZero())
            {

                upgradeTower(clickedTile);
                cout << "Tower upgraded via double-click!" << endl;
                doubleClickTimer.resetToZero(); // Reset to avoid multi-upgrade
            }
            else
            {
                // First click
                lastClickedTile = clickedTile;
                doubleClickTimer.resetToMax();
            }
            foundTower = true;
            cout << "Tower selected at (" << tilePos.x << ", " << tilePos.y << ")" << endl;
            break;
        }
    }

    if (!foundTower)
    {
        selectedTower = nullptr;
        sellConfirm = false;
        clickLockTimer.resetToMax();
    }
}

void Game::selectedTowerDisplay()
{
    // border mark of selected tower
    Vector2 pos = selectedTower->getPosition();
    DrawRectangleLinesEx({pos.x * tileSize, pos.y * tileSize,
                          (float)tileSize, (float)tileSize},
                         2, RED);
    int level = selectedTower->getTowerLevel();
    int cost = selectedTower->isMaxLevel() ? 0 : calculateUpgradeCost(selectedTower);
    int refund = selectedTower->getTotalSpent() * 0.6;

    std::string infoText = "Level: " + to_string(level) +
                           "  |  " + (cost == 0 ? "max level reached" : ("Upgrade: " + to_string(cost))) +
                           "  |  Refund: " + to_string(refund);

    // Screen position slightly above the hovered tower
    Vector2 center = {
        (selectedTower->getPosition().x + 0.5f) * tileSize,
        (selectedTower->getPosition().y - 0.3f) * tileSize};

    // text dimensions
    int textWidth = MeasureText(infoText.c_str(), 18);
    int textHeight = 20;
    Rectangle tooltipBox = {
        center.x - textWidth / 2.0f - 6,
        center.y - textHeight - 6,
        (float)textWidth + 12,
        (float)textHeight + 12};

    // background box and border
    DrawRectangleRec(tooltipBox, Fade(LIGHTGRAY, 0.9f));
    DrawRectangleLinesEx(tooltipBox, 1, DARKGRAY);

    // text inside the box
    DrawText(infoText.c_str(), tooltipBox.x + 6, tooltipBox.y + 6, 18, BLACK);

    // selling tooltip
    if (sellConfirm && selectedTower != nullptr)
    {
        DrawText("Right-click to SELL selected tower", 30, 90, 18, RED);
    }
}