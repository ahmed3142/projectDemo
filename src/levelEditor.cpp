#include "levelEditor.h"
#include "textureloader.h"
#include <bits/stdc++.h>
#include <iostream>
#include <raylib.h>
#include "leveldataio.h"
#include <fstream>

using namespace std;

LevelEditor::LevelEditor(int screenWidth, int screenHeight)
    : levelData(LevelData()),
      screenHeight(screenHeight),
      screenWidth(screenWidth),
      level(make_unique<Level>(31, 19))
{
    // cout << screenWidth/tileSize << " " << screenHeight/tileSize << "leveleditor" << endl;
}

void LevelEditor::run()
{
    bool running = true;
    while (!WindowShouldClose() && running)
    {
        processInput(running);
        draw();
    }
}

void LevelEditor::processInput(bool &running)
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        running = false; // Exit the editor
    }

    Vector2 mouse = GetMousePosition();
    Vector2 gridPosition = {mouse.x / tileSize, mouse.y / tileSize};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (IsKeyDown(KEY_ONE))
        {
            level->setTileWall((int)gridPosition.x, (int)gridPosition.y, true);
            levelData.wallPositions.push_back(gridPosition);
            cout << "wall added at " << gridPosition.x << " " << gridPosition.y << endl;
        }
        else if (IsKeyDown(KEY_TWO))
        {
            level->setTileWall((int)gridPosition.x, (int)gridPosition.y, false);
            level->setTileType((int)gridPosition.x, (int)gridPosition.y, TileType::enymyspawner);
            levelData.spawnerPositions.push_back(gridPosition);
            cout << "spawner added at " << gridPosition.x << " " << gridPosition.y << endl;
        }
        // else if (IsKeyDown(KEY_THREE)) {
        //      level->setTileWall((int)gridPosition.x, (int)gridPosition.y, false);
        //      levelData.targetTile = gridPosition;
        //      level->setTileType((int)gridPosition.x, (int)gridPosition.y, TileType::targettile);
        //      cout << "target added at " << gridPosition.x << " " << gridPosition.y << endl;
        //  }
    }
    if (IsKeyPressed(KEY_S)) {
    vector<LevelData> allLevels;
    ifstream infile("all_levels.json");
    if (infile.good()) {
        allLevels = loadAllLevelsFromFile("all_levels.json");
    }
    allLevels.push_back(levelData);
    saveAllLevelsToFile(allLevels, "all_levels.json");
    cout << "Appended new level to all_levels.json" << endl;
}

    // if (IsKeyPressed(KEY_C))
    // {
    //     levelData = LevelData(); // blank data
    //     // int tilesX = screenWidth / tileSize;
    //     // int tilesY = screenHeight / tileSize;

    //     //level = make_unique<Level>(31, 19);
    //     level->loadFromData(levelData);

    //     std::cout << "Cleared level. Start fresh." << std::endl;
    // }

    if (IsKeyPressed(KEY_E))
    {
        saveAllLevelsToFile(allLevels, "all_levels.json");
        std::cout << "All levels saved to all_levels.json" << std::endl;
    }
}

void LevelEditor::draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    level->draw(tileSize);
    EndDrawing();
}

LevelData LevelEditor::getLevelData() const
{
    return levelData;
}