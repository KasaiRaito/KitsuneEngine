#pragma once
#include "vector/Vector2D.h"

struct CollisionManifold2D {
    bool colliding = false;
    Vector2D normal;
    float penetration = 0.0f;
};
