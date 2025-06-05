#include "tower.h"
#include <raymath.h>
#include <algorithm>
#include "projectile.h"
#include <cmath>
#include <algorithm>   // for std::clamp

using namespace std;

const float Tower::angularSpeed = 180.0f * DEG2RAD * 5.0f; 
const float Tower::attackRange = 3.0f; // attack range in tiles

Tower::Tower(Vector2 setPosition) :
    position(setPosition), angle(0.0f), //angle in radians
    weaponTimer(1.0f) //  seconds cooldown for shooting
{
    textureTileTower = *TextureLoader::LoadTextureFromFile("Tile Tower.png");
}

void Tower::update(float deltaTime, vector<shared_ptr<Unit>>& units, vector<Projectile>& projectiles)
{
    weaponTimer.countDown(deltaTime); // update weapon timer
    
    // pick a target if we don’t have one
    if(auto e = targetEnemy.lock()){
        if(e-> getIsAlive() == false || Vector2Distance(position, e->getPosition()) > attackRange){
            targetEnemy.reset(); // clear target if it’s dead or out of range 
            // weak_ptr  
        }
    }

    if(targetEnemy.expired())
    {
        targetEnemy = findEnemy(units); // find a new target
    }

    if(updateAngle(deltaTime)){
        shoot(projectiles);
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
    float drawAngle = (angle * RAD2DEG) + 90; // angle in radians

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

bool Tower::updateAngle(float deltaTime) {
    if (auto e = targetEnemy.lock())
    {
        // 1) get the unit-to-tower vector, normalize it
        Vector2 x=e->getPosition();
        x.x-=0.5f;
        x.y-=0.5f; // center the unit on its tile

        Vector2 delta = Vector2Subtract(x, position);
        Vector2 dir   = Vector2Normalize(delta);

        // 2) absolute “desired” angle in world coords
        float desired = std::atan2f(dir.y, dir.x);

        // 3) find the shortest signed difference
        float diff = desired - angle;
        if (diff >  PI) diff -= 2*PI;
        if (diff < -PI) diff += 2*PI;

        // 4) clamp how much we can turn this frame
        float maxTurn = angularSpeed * deltaTime;
        if (fabsf(maxTurn) >= fabsf(diff)) {
        angle = desired;
        return true;      // We’re now pointing exactly at the target
    }


        // 5) apply it
        if(diff > 0) {
            angle += maxTurn; // turn clockwise
        } else {
            angle -= maxTurn; // turn counter-clockwise
        }

        // keep angle in –π..π or 0..2π so it never drifts wildly
        if (angle >  PI) angle -= 2*PI;
        if (angle < -PI) angle += 2*PI;
    }
    return false;
}

void Tower::shoot(vector<Projectile> &projectiles){
    if(weaponTimer.timeSIsZero()){
        Vector2 towerCenter = { position.x , position.y}; // center of the tower tile
        projectiles.push_back(Projectile(towerCenter, Vector2{cosf(angle), sinf(angle)}));
        weaponTimer.resetToMax(); // reset the timer after shooting
    }
}