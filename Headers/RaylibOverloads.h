//
// Created by iakin on 17/02/2025.
//

#ifndef RAYLIBOVERLOADS_H
#define RAYLIBOVERLOADS_H

#include <iostream>
#include <raylib.h>

inline std::ostream& operator<<(std::ostream& output, const Vector2& input)
{
    output << "Vector2(" << input.x << ", " << input.y << ')';
    return output;
}

inline bool operator==(const GridPos& gridPos1, const GridPos& gridPos2)
{
    return (gridPos1.x == gridPos2.x && gridPos1.y == gridPos2.y);
}

#endif //RAYLIBOVERLOADS_H
