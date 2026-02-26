#pragma once

#include "Component.h"
#include "vector/Vector2D.h"

class PhysicsComponent : public Component
{
public:
    bool enabled = true;
    bool useGravity = false;
    bool kinematic = false;
    bool allowRotation = true;

    float gravityScale = 1.0f;
    float linearDamping = 0.0f;
    float maxSpeed = -1.0f;
    float angularDamping = 0.0f;
    float maxAngularSpeed = -1.0f;
    float angularImpulseScale = 1.0f;
    float minAngularImpulse = 0.05f;
    float angularSleepThreshold = 0.08f;
    float angularSleepAccelerationThreshold = 0.15f;

    void SetMass(float m);
    float GetMass() const;
    float GetInvMass() const;

    void SetMomentOfInertia(float inertia);
    void SetMomentOfInertiaFromCircle(float radius);
    void SetMomentOfInertiaFromRectangle(float halfWidth, float halfHeight);
    float GetMomentOfInertia() const;
    float GetInvInertia() const;

    void AddForce(const Vector2D& force);
    void AddForceAtPoint(const Vector2D& force, const Vector2D& worldPoint);
    void AddImpulse(const Vector2D& impulse);
    void AddImpulseAtPoint(const Vector2D& impulse, const Vector2D& worldPoint);
    void AddTorque(float torque);
    void AddAngularImpulse(float angularImpulse);
    void ClearForces();

    const Vector2D& GetAccumulatedForce() const;
    float GetAccumulatedTorque() const;

    float GetAngularVelocity() const;
    void SetAngularVelocity(float value);

private:
    float mass = 1.0f;
    float momentOfInertia = 1.0f;
    bool inertiaOverridden = false;
    Vector2D accumulatedForce = Vector2D::Zero();
    float accumulatedTorque = 0.0f;
    float angularVelocity = 0.0f;
};
