#include "CollisionSystem2D.h"
#include <cmath>

// -------- Shape tests --------

static bool CircleVsCircle(
    const ColliderComponent2D& a,
    const ColliderComponent2D& b
) {
    if (!a.transform || !b.transform)
        return false;

    Vector2D delta =
        a.transform->location.value -
        b.transform->location.value;

    float r = a.circle.radius + b.circle.radius;
    return delta.Dot(delta) <= r * r;
}

// -------- Dispatcher --------

bool CheckCollision2D(
    const ColliderComponent2D& a,
    const ColliderComponent2D& b
) {
    if (a.shape == ColliderShape2D::None ||
        b.shape == ColliderShape2D::None)
        return false;

    if (a.shape == ColliderShape2D::Circle &&
        b.shape == ColliderShape2D::Circle) {
        return CircleVsCircle(a, b);
        }

    // Future:
    // Circle vs AABB
    // AABB vs AABB

    return false;
}