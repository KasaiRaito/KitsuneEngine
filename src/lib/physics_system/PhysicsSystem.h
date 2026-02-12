#pragma once

#include "vector/Vector2D.h"

class Object;
class PhysicsComponent;

class PhysicsSystem
{
public:
    static void SetGravity(const Vector2D& gravity);
    static const Vector2D& GetGravity();

    static void Simulate(Object& object, PhysicsComponent& physics, float dt);
};
