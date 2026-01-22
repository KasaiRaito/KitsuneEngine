#pragma once
#include "Component.h"
#include "Collider2D.h"
#include "ColliderTypes2D.h"

class CircleRenderComponent; // forward declare (or include if you prefer)

class ColliderComponent2D : public Component {
public:
    ColliderComponent2D(ColliderType2D type, void* shapeData);

    Collider2D* GetCollider() { return &collider; }
    void OnAdded() override;

private:
    Collider2D collider;
    CircleColliderData circleData{}; // ✅ owned data for circle colliders
};
