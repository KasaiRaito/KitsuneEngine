#include "ColliderComponent2D.h"
#include "Object.h"
#include "CircleRenderComponent.h" // so we can read render->radius
#include "SquareCollider2D.h"

ColliderComponent2D::ColliderComponent2D(ColliderType2D type, void* shapeData)
{
    collider.type = type;

    if (type == ColliderType2D::Circle)
    {
        // shapeData is expected to be a CircleRenderComponent*
        auto* render = static_cast<CircleRenderComponent*>(shapeData);
        circleData.radius = render ? render->radius : 0.0f;

        collider.data = &circleData; // ✅ correct type for DebugDraw()
    }
    else if (type == ColliderType2D::Square)
    {
        // shapeData is expected to be a SquareCollider2D*
        auto* square = static_cast<SquareCollider2D*>(shapeData);
        aabbData.halfWidth = square ? square->halfWidth : 0.0f;
        aabbData.halfHeight = square ? square->halfHeight : 0.0f;
        collider.data = &aabbData;
    }
    else
    {
        collider.data = nullptr;
    }
}

void ColliderComponent2D::OnAdded()
{
    collider.transform = &owner->transform;
    owner->collider = &collider;
}

void ColliderComponent2D::OnCollisionEnter(Object *other) {
    owner->OnCollisionEnter(other);
}

void ColliderComponent2D::OnCollisionStay(Object *other) {
    owner->OnCollisionStay(other);
}

void ColliderComponent2D::OnCollisionExit(Object *other) { // CHANGED
    owner->OnCollisionExit(other); // CHANGED
} // CHANGED

void ColliderComponent2D::OnTriggerEnter(Object *other) { // CHANGED
    owner->OnTriggerEnter(other); // CHANGED
} // CHANGED

void ColliderComponent2D::OnTriggerStay(Object *other) { // CHANGED
    owner->OnTriggerStay(other); // CHANGED
} // CHANGED

void ColliderComponent2D::OnTriggerExit(Object *other) { // CHANGED
    owner->OnTriggerExit(other); // CHANGED
} // CHANGED
