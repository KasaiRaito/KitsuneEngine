//
// Created by Eduardo Huerta on 12/02/26.
//

#ifndef KITSUNEENGINE_SQUARECOLLIDER2D_H
#define KITSUNEENGINE_SQUARECOLLIDER2D_H

#pragma once
#include "Collider2D.h"

class SquareCollider2D : public Collider2D
{
public:
    Vector2D* position = nullptr;
    float halfWidth = 20.0f;
    float halfHeight = 20.0f;

    SquareCollider2D(Vector2D* pos, float hw, float hh)
        : position(pos), halfWidth(hw), halfHeight(hh) {}

    void DebugDraw() const
    {
        if (!position) return;

        DrawRectangleLines(
            (int)(position->x - halfWidth),
            (int)(position->y - halfHeight),
            (int)(halfWidth * 2.0f),
            (int)(halfHeight * 2.0f),
            GREEN
        );
    }
};

#endif //KITSUNEENGINE_SQUARECOLLIDER2D_H
