#include "CollisionSystem2D.h"

CollisionManifold2D CollisionSystem2D::Test(
const Collider2D& a,
const Collider2D& b
) {
    if (a.type == ColliderType2D::Circle &&
        b.type == ColliderType2D::Circle) {
        return CircleVsCircle(a, b);
        }

    if (a.type == ColliderType2D::Square &&
        b.type == ColliderType2D::Square) {
        return SquareVsSquare(a, b);
        }

    // Mixed types later
    return {};
}
CollisionManifold2D CollisionSystem2D::CircleVsCircle(
    const Collider2D& a,
    const Collider2D& b
) {
    return {};
}

CollisionManifold2D CollisionSystem2D::SquareVsSquare(
    const Collider2D& a,
    const Collider2D& b
) {
    return {};
}
