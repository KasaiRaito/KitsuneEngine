#include "CollisionSystem2D.h"
#include <cmath>

static float Length(float x, float y) { return std::sqrt(x*x + y*y); }
static float Clamp(float v, float minV, float maxV)
{
    if (v < minV) return minV;
    if (v > maxV) return maxV;
    return v;
}

CollisionManifold2D CollisionSystem2D::Test(const Collider2D& a, const Collider2D& b)
{
    if (a.type == ColliderType2D::Circle && b.type == ColliderType2D::Circle)
        return CircleVsCircle(a, b);
    if (a.type == ColliderType2D::Circle && b.type == ColliderType2D::Square)
        return CircleVsSquare(a, b);
    if (a.type == ColliderType2D::Square && b.type == ColliderType2D::Circle)
        return SquareVsCircle(a, b);
    if (a.type == ColliderType2D::Square && b.type == ColliderType2D::Square)
        return SquareVsSquare(a, b);

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

CollisionManifold2D CollisionSystem2D::CircleVsSquare(const Collider2D& circle, const Collider2D& square)
{
    CollisionManifold2D out{};
    out.colliding = false;
    out.penetration = 0.0f;
    out.normal = {0, 0};

    if (!circle.transform || !square.transform) return out;
    if (!circle.data || !square.data) return out;

    auto* c = (const CircleColliderData*)circle.data;
    auto* box = (const AABBColliderData*)square.data;

    const float cx = circle.transform->location.value.x;
    const float cy = circle.transform->location.value.y;
    const float sx = square.transform->location.value.x;
    const float sy = square.transform->location.value.y;

    const float minX = sx - box->halfWidth;
    const float maxX = sx + box->halfWidth;
    const float minY = sy - box->halfHeight;
    const float maxY = sy + box->halfHeight;

    const float closestX = Clamp(cx, minX, maxX);
    const float closestY = Clamp(cy, minY, maxY);

    float dx = closestX - cx;
    float dy = closestY - cy;
    float dist = Length(dx, dy);

    if (dist < 0.00001f)
    {
        // Circle center is inside/very close to box: choose shallowest axis.
        const float leftPen = std::fabs(cx - minX);
        const float rightPen = std::fabs(maxX - cx);
        const float topPen = std::fabs(cy - minY);
        const float bottomPen = std::fabs(maxY - cy);

        float minPen = leftPen;
        out.normal = {-1.0f, 0.0f};

        if (rightPen < minPen) { minPen = rightPen; out.normal = {1.0f, 0.0f}; }
        if (topPen < minPen) { minPen = topPen; out.normal = {0.0f, -1.0f}; }
        if (bottomPen < minPen) { minPen = bottomPen; out.normal = {0.0f, 1.0f}; }

        out.colliding = true;
        out.penetration = c->radius + minPen;
        return out;
    }

    if (dist >= c->radius)
        return out;

    out.colliding = true;
    out.normal = { dx / dist, dy / dist }; // normal from circle -> square
    out.penetration = c->radius - dist;
    return out;
}

CollisionManifold2D CollisionSystem2D::SquareVsCircle(const Collider2D& square, const Collider2D& circle)
{
    CollisionManifold2D out = CircleVsSquare(circle, square);
    out.normal = out.normal * -1.0f; // flip to keep normal from A -> B
    return out;
}

CollisionManifold2D CollisionSystem2D::SquareVsSquare(const Collider2D& a, const Collider2D& b)
{
    CollisionManifold2D out{};
    out.colliding = false;
    out.penetration = 0.0f;
    out.normal = {0, 0};

    if (!a.transform || !b.transform) return out;
    if (!a.data || !b.data) return out;

    auto* aa = (const AABBColliderData*)a.data;
    auto* bb = (const AABBColliderData*)b.data;

    const float ax = a.transform->location.value.x;
    const float ay = a.transform->location.value.y;
    const float bx = b.transform->location.value.x;
    const float by = b.transform->location.value.y;

    const float dx = bx - ax;
    const float px = (aa->halfWidth + bb->halfWidth) - std::fabs(dx);
    if (px <= 0.0f) return out;

    const float dy = by - ay;
    const float py = (aa->halfHeight + bb->halfHeight) - std::fabs(dy);
    if (py <= 0.0f) return out;

    out.colliding = true;
    if (px < py)
    {
        out.penetration = px;
        out.normal = (dx >= 0.0f) ? Vector2D{1.0f, 0.0f} : Vector2D{-1.0f, 0.0f};
    }
    else
    {
        out.penetration = py;
        out.normal = (dy >= 0.0f) ? Vector2D{0.0f, 1.0f} : Vector2D{0.0f, -1.0f};
    }

    return out;
}
