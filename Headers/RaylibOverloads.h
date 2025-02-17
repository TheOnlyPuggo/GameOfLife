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

#endif //RAYLIBOVERLOADS_H
