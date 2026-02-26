#include "CollisionSystem2D.h"
#include <algorithm>
#include <cmath>
#include <limits>

static float Length(float x, float y) { return std::sqrt(x*x + y*y); }
static float Clamp(float v, float minV, float maxV)
{
    if (v < minV) return minV;
    if (v > maxV) return maxV;
    return v;
}

namespace
{
    static constexpr float kEpsilon = 0.00001f;

    struct Projection
    {
        float min = 0.0f;
        float max = 0.0f;
    };

    static Vector2D RotateVector(const Vector2D& value, float radians)
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        return { value.x * c - value.y * s, value.x * s + value.y * c };
    }

    static Vector2D NormalizeOrDefault(const Vector2D& value, const Vector2D& fallback)
    {
        const float len = value.Length();
        if (len <= kEpsilon)
            return fallback;

        return value * (1.0f / len);
    }

    static Projection ProjectPoints(const Vector2D points[4], const Vector2D& axis)
    {
        Projection projection{};
        projection.min = points[0].Dot(axis);
        projection.max = projection.min;

        for (int i = 1; i < 4; ++i)
        {
            const float value = points[i].Dot(axis);
            if (value < projection.min) projection.min = value;
            if (value > projection.max) projection.max = value;
        }

        return projection;
    }

    static bool BuildSquareData(const Collider2D& square,
                                Vector2D vertices[4],
                                Vector2D& axisX,
                                Vector2D& axisY)
    {
        if (!square.transform || !square.data)
            return false;

        auto* box = (const AABBColliderData*)square.data;
        const Vector2D center = square.transform->location.value;
        const float angle = square.transform->rotation.angle;

        axisX = NormalizeOrDefault(RotateVector({ 1.0f, 0.0f }, angle), { 1.0f, 0.0f });
        axisY = NormalizeOrDefault(RotateVector({ 0.0f, 1.0f }, angle), { 0.0f, 1.0f });

        const Vector2D local[4] = {
            { -box->halfWidth, -box->halfHeight },
            {  box->halfWidth, -box->halfHeight },
            {  box->halfWidth,  box->halfHeight },
            { -box->halfWidth,  box->halfHeight }
        };

        for (int i = 0; i < 4; ++i)
            vertices[i] = center + RotateVector(local[i], angle);

        return true;
    }
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
    out.hasContactPoint = false;

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
    if (dist <= kEpsilon)
    {
        out.normal = {1.0f, 0.0f};
        out.penetration = r;
        const Vector2D pointA = Vector2D{ ax, ay } + out.normal * ca->radius;
        const Vector2D pointB = Vector2D{ bx, by } - out.normal * cb->radius;
        out.contactPoint = (pointA + pointB) * 0.5f;
        out.hasContactPoint = true;
        return out;
    }

    // Normal from A -> B
    out.normal = { dx / dist, dy / dist };
    out.penetration = (r - dist);
    {
        const Vector2D pointA = Vector2D{ ax, ay } + out.normal * ca->radius;
        const Vector2D pointB = Vector2D{ bx, by } - out.normal * cb->radius;
        out.contactPoint = (pointA + pointB) * 0.5f;
        out.hasContactPoint = true;
    }
    return out;
}

CollisionManifold2D CollisionSystem2D::CircleVsSquare(const Collider2D& circle, const Collider2D& square)
{
    CollisionManifold2D out{};
    out.colliding = false;
    out.penetration = 0.0f;
    out.normal = {0, 0};
    out.hasContactPoint = false;

    if (!circle.transform || !square.transform) return out;
    if (!circle.data || !square.data) return out;

    auto* c = (const CircleColliderData*)circle.data;
    auto* box = (const AABBColliderData*)square.data;

    const Vector2D circleCenter = circle.transform->location.value;
    const Vector2D squareCenter = square.transform->location.value;
    const float squareAngle = square.transform->rotation.angle;

    const Vector2D localCircleCenter = RotateVector(circleCenter - squareCenter, -squareAngle);

    const float clampedX = Clamp(localCircleCenter.x, -box->halfWidth, box->halfWidth);
    const float clampedY = Clamp(localCircleCenter.y, -box->halfHeight, box->halfHeight);
    const Vector2D closestLocal = { clampedX, clampedY };
    const Vector2D toClosestLocal = closestLocal - localCircleCenter;

    const float dist = toClosestLocal.Length();

    if (dist < kEpsilon)
    {
        // Circle center is inside/very close to box: choose shallowest axis.
        const float distToRight = box->halfWidth - localCircleCenter.x;
        const float distToLeft = localCircleCenter.x + box->halfWidth;
        const float distToTop = localCircleCenter.y + box->halfHeight;
        const float distToBottom = box->halfHeight - localCircleCenter.y;

        float minPen = distToLeft;
        out.normal = {-1.0f, 0.0f};
        Vector2D contactLocal = { -box->halfWidth, Clamp(localCircleCenter.y, -box->halfHeight, box->halfHeight) };

        if (distToRight < minPen)
        {
            minPen = distToRight;
            out.normal = { 1.0f, 0.0f };
            contactLocal = { box->halfWidth, Clamp(localCircleCenter.y, -box->halfHeight, box->halfHeight) };
        }
        if (distToTop < minPen)
        {
            minPen = distToTop;
            out.normal = { 0.0f, -1.0f };
            contactLocal = { Clamp(localCircleCenter.x, -box->halfWidth, box->halfWidth), -box->halfHeight };
        }
        if (distToBottom < minPen)
        {
            minPen = distToBottom;
            out.normal = { 0.0f, 1.0f };
            contactLocal = { Clamp(localCircleCenter.x, -box->halfWidth, box->halfWidth), box->halfHeight };
        }

        out.colliding = true;
        out.penetration = c->radius + minPen;
        out.normal = NormalizeOrDefault(RotateVector(out.normal, squareAngle), { 1.0f, 0.0f });
        out.contactPoint = squareCenter + RotateVector(contactLocal, squareAngle);
        out.hasContactPoint = true;
        return out;
    }

    if (dist >= c->radius)
        return out;

    out.colliding = true;
    out.normal = NormalizeOrDefault(RotateVector(toClosestLocal * (1.0f / dist), squareAngle), { 1.0f, 0.0f }); // normal from circle -> square
    out.penetration = c->radius - dist;
    out.contactPoint = squareCenter + RotateVector(closestLocal, squareAngle);
    out.hasContactPoint = true;
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
    out.hasContactPoint = false;

    if (!a.transform || !b.transform) return out;
    if (!a.data || !b.data) return out;

    Vector2D verticesA[4]{};
    Vector2D verticesB[4]{};
    Vector2D axisAX{};
    Vector2D axisAY{};
    Vector2D axisBX{};
    Vector2D axisBY{};

    if (!BuildSquareData(a, verticesA, axisAX, axisAY))
        return out;
    if (!BuildSquareData(b, verticesB, axisBX, axisBY))
        return out;

    Vector2D axes[4] = { axisAX, axisAY, axisBX, axisBY };
    float minimumOverlap = std::numeric_limits<float>::max();
    Vector2D minimumAxis = { 1.0f, 0.0f };

    for (const Vector2D& rawAxis : axes)
    {
        const Vector2D axis = NormalizeOrDefault(rawAxis, { 1.0f, 0.0f });
        const Projection projectionA = ProjectPoints(verticesA, axis);
        const Projection projectionB = ProjectPoints(verticesB, axis);

        const float overlap = std::min(projectionA.max, projectionB.max) - std::max(projectionA.min, projectionB.min);
        if (overlap <= 0.0f)
            return out;

        if (overlap < minimumOverlap)
        {
            minimumOverlap = overlap;
            minimumAxis = axis;
        }
    }

    out.colliding = true;
    out.penetration = minimumOverlap;

    const Vector2D centerA = a.transform->location.value;
    const Vector2D centerB = b.transform->location.value;
    if ((centerB - centerA).Dot(minimumAxis) < 0.0f)
        minimumAxis = minimumAxis * -1.0f;

    out.normal = minimumAxis;

    const Vector2D tangent = { -out.normal.y, out.normal.x };
    const Projection projectionANormal = ProjectPoints(verticesA, out.normal);
    const Projection projectionBNormal = ProjectPoints(verticesB, out.normal);
    const Projection projectionATangent = ProjectPoints(verticesA, tangent);
    const Projection projectionBTangent = ProjectPoints(verticesB, tangent);

    const float contactNormal = (projectionANormal.max + projectionBNormal.min) * 0.5f;
    const float tangentMin = std::max(projectionATangent.min, projectionBTangent.min);
    const float tangentMax = std::min(projectionATangent.max, projectionBTangent.max);
    const float contactTangent = (tangentMin <= tangentMax)
        ? (tangentMin + tangentMax) * 0.5f
        : (projectionATangent.max + projectionBTangent.min) * 0.5f;

    out.contactPoint = out.normal * contactNormal + tangent * contactTangent;
    out.hasContactPoint = true;
    return out;
}
