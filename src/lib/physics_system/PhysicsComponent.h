#pragma once

#include "Component.h"
#include "vector/Vector2D.h"

class PhysicsComponent : public Component
{
public:
    bool enabled = true;
    bool useGravity = false;
    bool kinematic = false;

    float gravityScale = 1.0f;
    float linearDamping = 0.0f;
    float maxSpeed = -1.0f;

    void SetMass(float m);
    float GetMass() const;
    float GetInvMass() const;

    void AddForce(const Vector2D& force);
    void AddImpulse(const Vector2D& impulse);
    void ClearForces();

    const Vector2D& GetAccumulatedForce() const;

private:
    float mass = 1.0f;
    Vector2D accumulatedForce = Vector2D::Zero();
};
