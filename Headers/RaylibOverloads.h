//
// Created by iakin on 17/02/2025.
//

#ifndef RAYLIBOVERLOADS_H
#define RAYLIBOVERLOADS_H

#include <iostream>
#include <raylib.h>

inline Vector2 operator+(const Vector2& vec1, const Vector2& vec2)
{
    return Vector2(vec1.x + vec2.x, vec1.y + vec2.y);
}

inline Vector2 operator-(const Vector2& vec1, const Vector2& vec2)
{
    return Vector2(vec1.x - vec2.x, vec1.y - vec2.y);
}

inline std::ostream& operator<<(std::ostream& output, const Vector2& input)
{
    output << "Vector2(" << input.x << ", " << input.y << ')';
    return output;
}

inline bool operator==(const Vector2& vec1, const Vector2& vec2)
{
    return (vec1.x == vec2.x && vec1.y == vec2.y);
}

#endif //RAYLIBOVERLOADS_H
