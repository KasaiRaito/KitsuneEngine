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

    virtual void OnCollisionExit(Object* other); // CHANGED

    // trigger callbacks // CHANGED
    virtual void OnTriggerEnter(Object* other); // CHANGED

    virtual void OnTriggerStay(Object* other); // CHANGED

    virtual void OnTriggerExit(Object* other); // CHANGED

    Collider2D* GetCollider() { return &collider; }

    // keeps track of current collisions
    std::unordered_set<Object*> collidingWith;

private:
    Collider2D collider;
    CircleColliderData circleData{};
    AABBColliderData aabbData{};
};
