#include "unit.h"
#include "game.h"
#include <raymath.h>
#include "textureloader.h"
#include <bits/stdc++.h>
#include <memory>
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

void Unit::update(float deltaTime, Level &level, vector<shared_ptr<Unit>> &units)
{
    float targetDistance = Vector2Distance(level.getTargetPosition(), position);
    float moveDistance = speed * deltaTime;
    // cout << "Unit at position: (" << position.x << ", " << position.y << ") with target distance: " << targetDistance << endl;

    if(targetDistance < 1.5f)
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
        if(unitSelected != nullptr && unitSelected->checkOverlap(position, size) && unitSelected.get() != this)
        {
            // They overlap so check and see if this unit is moving towards or away from the unit it overlaps.
            Vector2 directionToOther = Vector2Subtract(unitSelected->position, position);
            // Ensure that they're not directly on top of each other.
            if (Vector2Length(directionToOther) > 0.35f) // spacing between enemies
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
        if (ok) {
			//Check if it needs to move in the x direction.  If so then check if the new x position, plus an amount of spacing 
			//(to keep from moving too close to the wall) is within a wall or not and update the position as required.
			const float spacing = 0.47f;
			int x = (int)(position.x + moveForward.x + copysign(spacing, moveForward.x));
			int y = (int)(position.y);
			if (moveForward.x != 0.0f && level.isTileWall(x, y) == false){
				position.x += moveForward.x;
                if(level.isTileWall(position.x,position.y+spacing)) position.y-=(0.03f);
                if(level.isTileWall(position.x,position.y-spacing)) position.y+=0.03f;
            }
			//Do the same for the y direction.
			x = (int)(position.x);
			y = (int)(position.y + moveForward.y + copysign(spacing, moveForward.y));
			if (moveForward.y != 0.0f && level.isTileWall(x, y) == false){
				position.y += moveForward.y;
                if(level.isTileWall(position.x+spacing,position.y)) position.x-=(0.03f);
                if(level.isTileWall(position.x-spacing,position.y)) position.x+=0.03f;
            }
		}
    
}

bool Unit::getIsAlive()
{
    return alive;
}
