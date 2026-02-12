#include "PhysicsComponent.h"
#include "Object.h"

void PhysicsComponent::SetMass(float m)
{
    if (m <= 0.0f)
    {
        mass = 0.0001f;
        return;
    }

    mass = m;
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

void PhysicsComponent::AddForce(const Vector2D& force)
{
    accumulatedForce += force;
}

void PhysicsComponent::AddImpulse(const Vector2D& impulse)
{
    if (!owner || kinematic) return;
    owner->velocity += impulse * GetInvMass();
}

void PhysicsComponent::ClearForces()
{
    accumulatedForce = Vector2D::Zero();
}

const Vector2D& PhysicsComponent::GetAccumulatedForce() const
{
    return accumulatedForce;
}
