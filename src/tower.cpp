#include "tower.h"
#include <raymath.h>
#include <algorithm>
#include "projectile.h"
#include <cmath>
#include <algorithm>   

using namespace std;

const float Tower::angularSpeed = 180.0f * DEG2RAD * 5.0f; 
const float Tower::attackRange = 5.0f; // attack range in tiles

Tower::Tower(Vector2 setPosition) :
    position(setPosition), angle(0.0f), //angle in radians
    weaponTimer(1.0f) //  seconds cooldown for shooting
{
    textureTileTower = *TextureLoader::LoadTextureFromFile("Tile Tower.png");
}

void Tower::update(float deltaTime, vector<shared_ptr<Unit>>& units, vector<Projectile>& projectiles)
{
    weaponTimer.countDown(deltaTime); // update weapon timer
    
    // picking target  
    if(auto e = targetEnemy.lock()){
        if(e-> getIsAlive() == false || Vector2Distance(position, e->getPosition()) > attackRange){
            targetEnemy.reset(); // clear target if it’s dead or out of range 
        }
    }

    if(targetEnemy.expired())
    {
        targetEnemy = findEnemy(units); //finding new target
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
    float drawAngle = (angle * RAD2DEG) + 88; // angle in degrees 

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

bool Tower::updateAngle(float deltaTime)
{
    if (auto e = targetEnemy.lock())
    {
        Vector2 T = position;

        // 2) enemy’s current center‐ed position:
        Vector2 U = e->getPosition();
        U.x -= 0.5f;   // since your units and towers are centered on tiles
        U.y -= 0.5f;

        // 3) enemy’s velocity:
        Vector2 v_o = e->getVelocity();

        // 4) projectile speed:
        float v_p = Projectile::speed; // speed of the projectile

        // 5) build quadratic a·t² + b·t + c = 0
        Vector2 toTarget = Vector2Subtract(U, T);
        float a = Vector2DotProduct(v_o, v_o) - v_p*v_p;
        float b = 2.0f * Vector2DotProduct(toTarget, v_o);
        float c = Vector2DotProduct(toTarget, toTarget);

        // 6) discriminant
        float disc = b*b - 4.0f*a*c;
        Vector2 aimPoint;

        if (disc > 0.0f)
        {
            float sqrtD = sqrtf(disc);
            float t1 = (-b + sqrtD)/(2.0f*a);
            float t2 = (-b - sqrtD)/(2.0f*a);
            float t = FLT_MAX;
            if (t1>0) t = t1;
            if (t2>0 && t2<t) t = t2;

            if (t<FLT_MAX)
            {
                // intercept point
                aimPoint = Vector2Add(U, Vector2Scale(v_o, t));
            }
            else
            {
                aimPoint = U;  // fallback
            }
        }
        else
        {
            aimPoint = U;      // no real solution → fallback
        }

        // 7) compute desired angle
        Vector2 dir = Vector2Normalize(Vector2Subtract(aimPoint, T));
        float desired = atan2f(dir.y, dir.x);

        // 8) turn smoothly toward it
        float diff = desired - angle;
        if (diff >  PI) diff -= 2*PI;
        if (diff < -PI) diff += 2*PI;

        float maxTurn = angularSpeed * deltaTime;
        if (fabsf(diff) <= maxTurn)
        {
            angle = desired;
            return true;   // “locked on” → you can shoot this frame
        }
        angle += (diff>0 ? +maxTurn : -maxTurn);
        if (angle >  PI) angle -= 2*PI;
        if (angle < -PI) angle += 2*PI;
    }
    return false;
}

void Tower::shoot(vector<Projectile> &projectiles){
    if(weaponTimer.timeSIsZero()){
        Vector2 towerCenter = { position.x, position.y}; // center of the tower tile
        projectiles.push_back(Projectile(towerCenter, Vector2{cosf(angle), sinf(angle)}));
        weaponTimer.resetToMax(); // reset the timer after shooting
    }
}