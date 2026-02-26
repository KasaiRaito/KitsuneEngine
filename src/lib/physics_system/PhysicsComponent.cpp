#include "PhysicsComponent.h"
#include "Object.h"

#include <algorithm>
#include <cmath>

namespace
{
    static float Cross2D(const Vector2D& a, const Vector2D& b)
    {
        return (a.x * b.y) - (a.y * b.x);
    }
}

void PhysicsComponent::SetMass(float m)
{
    if (m <= 0.0f)
    {
        mass = 0.0001f;
    }
    else
    {
        mass = m;
    }

    if (!inertiaOverridden)
        momentOfInertia = std::max(0.0001f, mass);
}

float PhysicsComponent::GetMass() const
{
    return mass;
}

float PhysicsComponent::GetInvMass() const
{
    if (kinematic) return 0.0f;
    return (mass > 0.0f) ? (1.0f / mass) : 0.0f;
}

void PhysicsComponent::SetMomentOfInertia(float inertia)
{
    momentOfInertia = std::max(0.0001f, inertia);
    inertiaOverridden = true;
}

void PhysicsComponent::SetMomentOfInertiaFromCircle(float radius)
{
    const float clampedRadius = std::max(0.0001f, radius);
    const float inertia = 0.5f * mass * clampedRadius * clampedRadius;
    SetMomentOfInertia(inertia);
}

void PhysicsComponent::SetMomentOfInertiaFromRectangle(float halfWidth, float halfHeight)
{
    const float width = std::max(0.0001f, halfWidth * 2.0f);
    const float height = std::max(0.0001f, halfHeight * 2.0f);
    const float inertia = (mass * (width * width + height * height)) / 12.0f;
    SetMomentOfInertia(inertia);
}

float PhysicsComponent::GetMomentOfInertia() const
{
    return momentOfInertia;
}

float PhysicsComponent::GetInvInertia() const
{
    if (kinematic || !allowRotation)
        return 0.0f;

    return (momentOfInertia > 0.0f) ? (1.0f / momentOfInertia) : 0.0f;
}

void PhysicsComponent::AddForce(const Vector2D& force)
{
    accumulatedForce += force;
}

void PhysicsComponent::AddForceAtPoint(const Vector2D& force, const Vector2D& worldPoint)
{
    AddForce(force);

    if (!owner || !allowRotation)
        return;

    const Vector2D radius = worldPoint - owner->transform.location.value;
    AddTorque(Cross2D(radius, force));
}

void PhysicsComponent::AddImpulse(const Vector2D& impulse)
{
    if (!owner || kinematic) return;
    owner->velocity += impulse * GetInvMass();
}

void PhysicsComponent::AddImpulseAtPoint(const Vector2D& impulse, const Vector2D& worldPoint)
{
    AddImpulse(impulse);

    if (!owner || kinematic || !allowRotation)
        return;

    const Vector2D radius = worldPoint - owner->transform.location.value;
    AddAngularImpulse(Cross2D(radius, impulse));
}

void PhysicsComponent::AddTorque(float torque)
{
    if (kinematic || !allowRotation)
        return;

    accumulatedTorque += torque;
}

void PhysicsComponent::AddAngularImpulse(float angularImpulse)
{
    if (kinematic || !allowRotation)
        return;

    const float scaledImpulse = angularImpulse * angularImpulseScale;
    if (std::fabs(scaledImpulse) < minAngularImpulse)
        return;

    angularVelocity += scaledImpulse * GetInvInertia();
}

void PhysicsComponent::ClearForces()
{
    accumulatedForce = Vector2D::Zero();
    accumulatedTorque = 0.0f;
}

const Vector2D& PhysicsComponent::GetAccumulatedForce() const
{
    return accumulatedForce;
}

float PhysicsComponent::GetAccumulatedTorque() const
{
    return accumulatedTorque;
}

float PhysicsComponent::GetAngularVelocity() const
{
    return angularVelocity;
}

void PhysicsComponent::SetAngularVelocity(float value)
{
    angularVelocity = value;
}
