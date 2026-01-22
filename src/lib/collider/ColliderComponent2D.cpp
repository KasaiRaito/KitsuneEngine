#include "ColliderComponent2D.h"
#include "Object.h"
#include "CircleRenderComponent.h" // so we can read render->radius

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
    else
    {
        collider.data = nullptr;
    }
}

void ColliderComponent2D::OnAdded()
{
    collider.transform = &owner->transform; // ✅ fix null
}
