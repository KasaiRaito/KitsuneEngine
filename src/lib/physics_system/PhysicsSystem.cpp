#include "PhysicsSystem.h"

#include "Object.h"
#include "PhysicsComponent.h"

namespace
{
    Vector2D g_gravity = { 0.0f, 600.0f };
}

void PhysicsSystem::SetGravity(const Vector2D& gravity)
{
    g_gravity = gravity;
}

const Vector2D& PhysicsSystem::GetGravity()
{
    return g_gravity;
}

void PhysicsSystem::Simulate(Object& object, PhysicsComponent& physics, float dt)
{
    if (!physics.enabled || dt <= 0.0f)
    {
        physics.ClearForces();
        return;
    }

    if (physics.kinematic)
    {
        physics.ClearForces();
        return;
    }

    Vector2D acceleration = physics.GetAccumulatedForce() * physics.GetInvMass();

    if (physics.useGravity)
        acceleration += (g_gravity * physics.gravityScale);

    object.velocity += acceleration * dt;

    if (physics.linearDamping > 0.0f)
    {
        float dampingFactor = 1.0f - (physics.linearDamping * dt);
        if (dampingFactor < 0.0f) dampingFactor = 0.0f;
        object.velocity = object.velocity * dampingFactor;
    }

    if (physics.maxSpeed > 0.0f)
    {
        const float speed = object.velocity.Length();
        if (speed > physics.maxSpeed)
            object.velocity = object.velocity.Normalize() * physics.maxSpeed;
    }

    object.transform.location.Translate(object.velocity * dt);
    physics.ClearForces();
}
