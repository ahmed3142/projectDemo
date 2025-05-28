#include "tower.h"
#include <raymath.h>


const float Tower::angularSpeed = 180.0f * DEG2RAD; 

Tower::Tower(Vector2 setPosition) :
    position(setPosition), angle(0.0f)
{
    textureTileTower = *TextureLoader::LoadTextureFromFile("Tile Tower.png");
}

void Tower::update(float deltaTime)
{
    angle += angularSpeed * deltaTime;
    if (angle >= 360.f * DEG2RAD) angle -= 360.f * DEG2RAD; 
}

void Tower::draw(int tileSize)
{
    // Origin for rotation: center of the destination rectangle (half tileSize)
    Vector2 origin = { tileSize / 2.0f, tileSize / 2.0f };

    // Destination rectangle centered at (position.x * tileSize, position.y * tileSize)
    Rectangle destRect = {
        position.x * tileSize + origin.x,
        position.y * tileSize + origin.y,
        (float)tileSize,
        (float)tileSize
    };

    Rectangle sourceRect = { 0.0f, 0.0f, (float)textureTileTower.width, (float)textureTileTower.height };
    DrawTexturePro(textureTileTower, sourceRect, destRect, origin, angle * RAD2DEG, WHITE);
}


bool Tower::checkIfOnTile(int x,int y){
    return ((int)position.x == x && (int)position.y == y);
}