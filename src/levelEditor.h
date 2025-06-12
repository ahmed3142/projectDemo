#pragma once
#include "level.h"
#include "leveldata.h"
#include "leveldataio.h"
#include <bits/stdc++.h>
#include <memory>
#include <vector>

using namespace std;

class LevelEditor {
    unique_ptr<Level> level;
    LevelData levelData;
    vector<LevelData> allLevels;

    int tileSize = 48;

    int screenWidth;
    int screenHeight;

public:
    LevelEditor(int screenWidth, int screenHeight);
    void run();
    void processInput(bool &running);
    void draw();
    LevelData getLevelData() const;
};