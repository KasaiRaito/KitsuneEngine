#include "CollisionSystem2D.h"
#include <cmath>

static float Length(float x, float y) { return std::sqrt(x*x + y*y); }

CollisionManifold2D CollisionSystem2D::Test(const Collider2D& a, const Collider2D& b)
{
    if (a.type == ColliderType2D::Circle && b.type == ColliderType2D::Circle)
        return CircleVsCircle(a, b);

    return {};
}

CollisionManifold2D CollisionSystem2D::CircleVsCircle(const Collider2D& a, const Collider2D& b)
{
    CollisionManifold2D out{};
    out.colliding = false;
    out.penetration = 0.0f;
    out.normal = {0, 0};

    if (!a.transform || !b.transform) return out;
    if (!a.data || !b.data) return out;

    auto* ca = (const CircleColliderData*)a.data;
    auto* cb = (const CircleColliderData*)b.data;

    const float ax = a.transform->location.value.x;
    const float ay = a.transform->location.value.y;
    const float bx = b.transform->location.value.x;
    const float by = b.transform->location.value.y;

    const float dx = bx - ax;
    const float dy = by - ay;

    const float dist = Length(dx, dy);
    const float r = ca->radius + cb->radius;

    if (dist >= r)
        return out;

    out.colliding = true;

    // If centers are exactly overlapping, choose an arbitrary normal
    if (dist <= 0.00001f)
    {
        out.normal = {1.0f, 0.0f};
        out.penetration = r;
        return out;
    }

    // Normal from A -> B
    out.normal = { dx / dist, dy / dist };
    out.penetration = (r - dist);
    return out;
}
