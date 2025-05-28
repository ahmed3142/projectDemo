#pragma once
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <inttypes.h>
#include <queue>
#include <vector>
#include "textureloader.h"

using namespace std;

class Level
{
private:
    enum class TileType{
        empty,
        wall,
        enymyspawner
    };
    static const unsigned char MAX_LEVELS = 255;

    struct Tile
    {
        TileType type = TileType::empty;
        int directionX = 0; // flow direction
        int directionY = 0;
        bool isWall = false;
        // bool isSpawn = false;
        unsigned char flowDistance = MAX_LEVELS;
    };

    // void drawTile(int x, int y, int tileSize);
    void calculateFlowDistances();
    void calculateFlowField();
    void CalculateFlowFieldForTile(); // flow directions

    TileType getTileType(int x, int y); 
    void setTileType(int x,int y,TileType type);

    vector<Tile> tiles;
    //vector<Tile> tiles48; 
    const int tileCountX;
    const int tileCountY;
    const int targetTileX;
    const int targetTileY;

    Texture2D textureTileWall;
    Texture2D textureTileTarget;
    Texture2D textureTileEmpty;
    Texture2D textureTileArrowUp;
    Texture2D textureTileArrowUpRight;
    Texture2D textureTileArrowRight;
    Texture2D textureTileArrowDownRight;
    Texture2D textureTileArrowDown;
    Texture2D textureTileArrowDownLeft;
    Texture2D textureTileArrowLeft;
    Texture2D textureTileArrowUpLeft;
    Texture2D textureTileEnemySpawner;

public:
    Level(int setTileCountX, int setTileCountY);
    ~Level();

    Vector2 getRandomEnemySpawnerPosition();

    void draw(int tileSize);
    void drawTile(int x, int y, int tileSize);
    bool isTileWall(int x, int y);
    void setTileWall(int x, int y, bool isWall);
    Vector2 getTargetPosition();
    Vector2 getFlowNormal(int x, int y);
};