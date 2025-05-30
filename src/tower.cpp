#include "tower.h"
#include <raymath.h>
#include <algorithm>

using namespace std;

static inline float Vector2AngleBetween(Vector2 v1, Vector2 v2) {
    // 2D cross product “scalar”
    float cross = v1.x*v2.y - v1.y*v2.x;
    // 2D dot product
    float dot   = v1.x*v2.x + v1.y*v2.y;
    return atan2f(cross, dot);
}

const float Tower::angularSpeed = 180.0f * DEG2RAD * 5.0f; 
const float Tower::attackRange = 5.0f; // attack range in tiles

Tower::Tower(Vector2 setPosition) :
    position(setPosition), angle(0.0f) //angle in radians
{
    textureTileTower = *TextureLoader::LoadTextureFromFile("Tile Tower.png");
}

#include <cmath>
#include <algorithm>   // for std::clamp

void Tower::update(float deltaTime, std::vector<std::shared_ptr<Unit>>& units)
{
    // pick a target if we don’t have one
    if (targetEnemy.expired())
        targetEnemy = findEnemy(units);

    if (auto e = targetEnemy.lock())
    {
        // 1) get the unit-to-tower vector, normalize it
        Vector2 delta = Vector2Subtract(e->getPosition(), position);
        Vector2 dir   = Vector2Normalize(delta);

        // 2) absolute “desired” angle in world coords
        float desired = std::atan2f(dir.y, dir.x);

        // 3) find the shortest signed difference
        float diff = desired - angle;
        if (diff >  PI) diff -= 2*PI;
        if (diff < -PI) diff += 2*PI;

        // 4) clamp how much we can turn this frame
        float maxTurn = angularSpeed * deltaTime;
        if(diff > maxTurn) diff = maxTurn;
        else if(diff < -maxTurn) diff = -maxTurn;


        // 5) apply it
        angle += diff;

        // (optional) keep angle in –π..π or 0..2π so it never drifts wildly
        if (angle >  PI) angle -= 2*PI;
        if (angle < -PI) angle += 2*PI;
    }
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
    float drawAngle = (angle * RAD2DEG) + 92; // angle in radians

    Rectangle sourceRect = { 0.0f, 0.0f, (float)textureTileTower.width, (float)textureTileTower.height };
    DrawTexturePro(textureTileTower, sourceRect, destRect, origin, drawAngle, WHITE);
}


bool Tower::checkIfOnTile(int x,int y){
    return ((int)position.x == x && (int)position.y == y);
}

weak_ptr<Unit> Tower::findEnemy(vector<shared_ptr<Unit>>& units)
{
    weak_ptr<Unit> closestEnemy;
    float closestDistance = 0.0f;

    for(auto &unit : units){
        if(unit != nullptr){
            float currentDistance = Vector2Distance(position, unit->getPosition());
            if(currentDistance <= attackRange && (closestEnemy.expired() || currentDistance < closestDistance)){
                closestEnemy = unit;
                closestDistance = currentDistance;
            }
        }
    }

    return closestEnemy;
}