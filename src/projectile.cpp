#include "projectile.h"

const float Projectile::speed = 10.0f;
const float Projectile::size = 0.2f;
const float Projectile::maxDistance = 5.0f;

Projectile::Projectile(Vector2 setPosition, Vector2 setDirection)
    : position(setPosition), direction(Vector2Normalize(setDirection))
{
    bulletTex= *TextureLoader::LoadTextureFromFile("Bullet2.png");
}

void Projectile::update(float deltaTime){
    float moveDistance = speed * deltaTime;
    position+= direction * moveDistance;
    distanceTraveled += moveDistance;
    if (distanceTraveled >= maxDistance) {
        collided = true; // Mark as collided if max distance is reached
    }
}

void Projectile::draw(int tileSize) {
    // Get the texture’s pixel dimensions:
    int w = bulletTex.width;
    int h = bulletTex.height;

    // Compute screen position in pixels, centering the texture on (pos.x, pos.y):
    float drawX = position.x * tileSize;
    float drawY = position.y * tileSize;

    DrawTexture(bulletTex, (int)drawX, (int)drawY, WHITE);
}

bool Projectile::checkCollision() {
    return collided;
}
