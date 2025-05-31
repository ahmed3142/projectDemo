#pragma once
#include <raylib.h>
#include <bits/stdc++.h>
#include "unit.h"
#include <memory>
#include "textureloader.h"
#include <raymath.h>
//using namespace std;


class Projectile {
    Vector2 position;
    Vector2 direction;
    float distanceTraveled = 0.0f;

    Texture2D bulletTex; // Texture for the projectile

    bool collided = false;

    static const float speed; // Speed of the projectile
    static const float size; // Size of the projectile
    static const float maxDistance; // Maximum distance the projectile can travel

    public:
        Projectile(Vector2 setPosition, Vector2 setDirection);
        void update(float deltaTime);
        void draw(int tileSize);
        bool checkCollision();
};