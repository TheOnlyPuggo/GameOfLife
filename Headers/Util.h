//
// Created by iakin on 17/02/2025.
//

#ifndef UTIL_H
#define UTIL_H

#include <raylib.h>

struct GridPos
{
    int x{};
    int y{};
};

struct GridConversions
{
    static GridPos GetWorldToGridSpace(const Vector2& pos, const int cellSize)
    {
        return GridPos(std::floor(pos.x / (float)cellSize), std::floor(pos.y / (float)cellSize));
    }

    static Vector2 GetGridToWorldSpace(const GridPos& pos, const int cellSize)
    {
        return Vector2{(float)pos.x * (float)cellSize, (float)pos.y * (float)cellSize};
    }

    static Vector2 GetWorldGridNormalized(const Vector2& pos, const int cellSize)
    {
        return GetGridToWorldSpace(GetWorldToGridSpace(pos, cellSize), cellSize);
    }
};


#endif //UTIL_H
