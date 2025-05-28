#include "unit.h"
#include "game.h"
#include <raymath.h>
#include "textureloader.h"
#include <iostream>
#include <cmath>

const float Unit::speed = 5.0f;
const float Unit::size = 0.4f;

Unit::Unit(Vector2 setPosition) : position(setPosition)
{
    Texture2D *unitTex = TextureLoader::LoadTextureFromFile("Unit2.bmp");
    if (!unitTex)
    {
        std::cerr << "Failed to load one or more textures." << std::endl;
        exit(1); // or handle error gracefully
    }
    texture = *unitTex;
}

void Unit::draw(int tileSize)
{
    DrawTexturePro(texture, {0.0f, 0.0f, (float)texture.width, (float)texture.height},
                   {(position.x - size / 2) * tileSize, (position.y - size / 2) * tileSize, size * tileSize, size * tileSize},
                   {0.0f, 0.0f}, 0.0f, WHITE);
    // cout << "Unit at position: (" << position.x << ", " << position.y << ")" << endl;
}

bool Unit::checkOverlap(Vector2 positionOfOtherUnit, float otherUnitSize)
{
    if (Vector2Distance(positionOfOtherUnit, position) <= (otherUnitSize + size) / 2.0f)
        return true;
    else
        return false;
}

void Unit::update(float deltaTime, Level &level, vector<Unit> &units)
{
    float targetDistance = Vector2Distance(level.getTargetPosition(), position);
    float moveDistance = speed * deltaTime;
    // cout << "Unit at position: (" << position.x << ", " << position.y << ") with target distance: " << targetDistance << endl;

    if(targetDistance < 0.5f)
    {
        alive=false;
    }

    if (moveDistance > targetDistance)
        moveDistance = targetDistance;

    Vector2 normalDirection(level.getFlowNormal((int)position.x, (int)position.y));
    if((int) position.x == (int)level.getTargetPosition().x && (int) position.y == (int)level.getTargetPosition().y)
    {
        normalDirection= Vector2Subtract(level.getTargetPosition(), position);
        normalDirection = Vector2Normalize(normalDirection);
    }
    Vector2 moveForward = normalDirection * moveDistance;

    // moving forward without bumping other unit
    bool ok = true;
    for (int count = 0; count < units.size() && ok; count++)
    {
        auto &unitSelected = units[count];
        if (&unitSelected != this && unitSelected.checkOverlap(position, size))
        {
            // They overlap so check and see if this unit is moving towards or away from the unit it overlaps.
            Vector2 directionToOther = Vector2Subtract(unitSelected.position, position);
            // Ensure that they're not directly on top of each other.
            if (Vector2Length(directionToOther) > 0.01f)
            {
                // Check the angle between the units positions and the direction that this unit is traveling.
                // Ensure that this unit isn't moving directly towards the other unit (by checking the angle between).
                Vector2 normalToOther = Vector2Normalize(directionToOther);
                float angleBtw = fabs(Vector2Angle(normalToOther, normalDirection));
                if (angleBtw < 3.14159265359f / 4.0f) // tan45
                    // Don't allow the move.
                    ok = false;
            }
        }
    }
    // unit can move without walking through wall;
    if (ok)
    {
        const float spacing = 0.3f;

        int signX = (moveForward.x == 0) ? 0 : (moveForward.x > 0 ? 1 : -1);
        int signY = (moveForward.y == 0) ? 0 : (moveForward.y > 0 ? 1 : -1);

        // Check horizontal move with spacing
        int x = (int)(position.x + moveForward.x + signX * spacing);
        int y = (int)(position.y);
        bool canMoveX = (moveForward.x == 0) || !level.isTileWall(x, y);

        // Check vertical move with spacing
        x = (int)(position.x);
        y = (int)(position.y + moveForward.y + signY * spacing);
        bool canMoveY = (moveForward.y == 0) || !level.isTileWall(x, y);

        // Additional corner check for diagonal movement
        bool canMoveCorner = true;
        if (moveForward.x != 0 && moveForward.y != 0)
        {
            x = (int)(position.x + signX * spacing);
            y = (int)(position.y + signY * spacing);
            if (level.isTileWall(x, y))
            {
                canMoveCorner = false;
            }
        }

        // Apply movement only if no walls blocking
        if (canMoveX)
        {
            position.x += moveForward.x;
        }
        if (canMoveY)
        {
            position.y += moveForward.y;
        }

        // If diagonal movement attempted, and corner is blocked,
        // prevent diagonal movement by cancelling one axis:
        if (moveForward.x != 0 && moveForward.y != 0 && !canMoveCorner)
        {
            // For example, prevent diagonal slip by moving only along one axis
            // Try horizontal only if vertical blocked
            if (!canMoveY && canMoveX)
            {
                position.y -= moveForward.y; // rollback vertical move
            }
            // Or try vertical only if horizontal blocked
            else if (!canMoveX && canMoveY)
            {
                position.x -= moveForward.x; // rollback horizontal move
            }
            // Otherwise, rollback both
            else if (!canMoveX && !canMoveY)
            {
                position.x -= moveForward.x;
                position.y -= moveForward.y;
            }
        }
    }
}

bool Unit::isAlive()
{
    return alive;
}
