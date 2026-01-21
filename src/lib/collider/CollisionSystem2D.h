#pragma once
#include "Collider2D.h"
#include "CollisionManifold2D.h"

class CollisionSystem2D {
public:
    static CollisionManifold2D Test(const Collider2D& a, const Collider2D& b);

private:
    static CollisionManifold2D CircleVsCircle(
        const Collider2D& a,
        const Collider2D& b
    );

    static CollisionManifold2D SquareVsSquare(
        const Collider2D& a,
        const Collider2D& b
    );
};
