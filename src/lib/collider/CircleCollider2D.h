//
// Created by Eduardo Huerta on 21/01/26.
//

#ifndef KITSUNEENGINE_CIRCLECOLLIDER2D_H
#define KITSUNEENGINE_CIRCLECOLLIDER2D_H

#pragma once
#include "Collider2D.h"
#include "Vector2D.h"

class CircleCollider2D : public Collider2D
{
public:
    Vector2D* position = nullptr;
    float radius = 20.0f;

    CircleCollider2D(Vector2D* pos, float r)
        : position(pos), radius(r) {}

    void DebugDraw() const
    {
        if (!position) return;

        DrawCircleLines(
            (int)position->x,
            (int)position->y,
            radius,
            GREEN
        );
    }
};

#endif //KITSUNEENGINE_CIRCLECOLLIDER2D_H