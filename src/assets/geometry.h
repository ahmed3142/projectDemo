#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <raylib.h>
#include <cmath>

using namespace std;

namespace geometrics{
    constexpr float DtoR = 0.0174533f; // Degrees to Radians
    constexpr float RtoD = 57.2958f;   // Radians to Degrees
    constexpr float SQRT2 = 1.41421356237f;

    static float vLength(const Vector2& v){
        return sqrt(v.x*v.x + v.y*v.y);
    }

    static Vector2 vNormalize(const Vector2& v){
        float l=vLength(v);
        if(l == 0.0f) return v;
        return {v.x/l , v.y/l};
    }
     /// Returns the clockwise angle in degrees from 0-360 relative to the down axis Vector2(0, 1)
    static float Angle(const Vector2& a)
    {
        // Use atan2f to get the angle relative to the X and Y axes
        // Vector2(0, 1) is the "down" direction (which corresponds to angle 0 degrees)
        float angle = std::atan2f(a.x, a.y);  // atan2 takes (y, x)
        
        // Convert the angle from radians to degrees
        angle *= RtoD;

        // Ensure the angle is between 0 and 360 degrees
        if (angle < 0.0f)
        {
            angle += 360.0f;
        }

        return angle;
    }
}

#endif