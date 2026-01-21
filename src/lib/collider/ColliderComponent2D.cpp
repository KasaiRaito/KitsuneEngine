#include "ColliderComponent2D.h"
#include "Object.h"

ColliderComponent2D::ColliderComponent2D(ColliderType2D type, void* shapeData) {
    collider.type = type;
    collider.data = shapeData;
}

void ColliderComponent2D::OnAdded() {
    Component::OnAdded();

    owner->collider = &collider;
    collider.position = &owner->transform.location.value; // adjust to your actual Vector2D field

}
