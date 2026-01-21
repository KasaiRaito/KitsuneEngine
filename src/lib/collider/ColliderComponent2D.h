#pragma once
#include "Component.h"
#include "Collider2D.h"

class ColliderComponent2D : public Component {
public:
    ColliderComponent2D(ColliderType2D type, void* shapeData);

    Collider2D* GetCollider() { return &collider; }

    void OnAdded() override;

private:
    Collider2D collider;
};
