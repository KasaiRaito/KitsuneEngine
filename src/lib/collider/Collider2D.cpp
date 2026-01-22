#include "Collider2D.h"

void Collider2D::DebugDraw() const
{
    if (!transform || !data) return;

    switch (type)
    {
        case ColliderType2D::Circle:
        {
            auto* c = (CircleColliderData*)data;
            DrawCircleLines(transform->location.x(), transform->location.y(), c->radius, GREEN);
        } break;

        default:
            break;
    }
}
