#pragma once
#include "Component.h"
#include "Collider2D.h"
#include "ColliderTypes2D.h"
#include <unordered_set>

class ColliderComponent2D : public Component
{
public:
    ColliderComponent2D(ColliderType2D type, void* shapeData);

    void OnAdded() override;

    // collision callbacks
    virtual void OnCollisionEnter(Object* other);

    virtual void OnCollisionStay(Object* other);

    virtual void OnCollisionExit(Object* other) {}

    Collider2D* GetCollider() { return &collider; }

    // keeps track of current collisions
    std::unordered_set<Object*> collidingWith;

private:
    Collider2D collider;
    CircleColliderData circleData{};
};
