#include "projectile.h"

const float Projectile::speed = 3.0f;
const float Projectile::size = 0.2f;
const float Projectile::maxDistance = 5.0f;

Projectile::Projectile(Vector2 setPosition, Vector2 setDirection)
    : position(setPosition), direction(Vector2Normalize(setDirection))
{
    bulletTex= *TextureLoader::LoadTextureFromFile("Bullet2.png");
}

void Projectile::update(float deltaTime, vector<shared_ptr<Unit>> units){
    float moveDistance = speed * deltaTime;
    position+= direction * moveDistance;
    distanceTraveled += moveDistance;
    if (distanceTraveled >= maxDistance) {
        collided = true; // Marking as collided if max distance is reached
    }

    checkCollisionWithEnemy(units);
}

void Projectile::draw(int tileSize) {
    // Get the texture’s pixel dimensions:
    // int w = bulletTex.width;
    // int h = bulletTex.height;

    // Compute screen position in pixels, centering the texture on (pos.x, pos.y):
    float drawX = position.x * tileSize;
    float drawY = position.y * tileSize;
    
    DrawTexture(bulletTex, (int)drawX, (int)drawY, WHITE);
    DrawCircleV(Vector2{(position.x + 0.5f) * tileSize , (position.y + 0.5f) * tileSize}, (size * tileSize) / 2.0f, Fade(RED, 0.2f));
}

bool Projectile::checkCollision() {
    return collided;
}

void Projectile::checkCollisionWithEnemy(vector<shared_ptr<Unit>> units){
    if(!collided){
        for(int i=0; i<(int)units.size() && collided==false; i++){
            auto &unit = units[i];
            // cout << unit->getCurrentHealth() << " " << i << endl; 
            Vector2 projectilePosition;
            projectilePosition.x = position.x + 0.5f;
            projectilePosition.y = position.y + 0.5f;
            if(unit != nullptr && unit->getIsAlive() && unit->checkOverlap(projectilePosition, size)){
                unit->damage(1);
                collided = true;
            }
        }
    }
}

float Projectile::getProjectileSpeed() {
    return speed;
}