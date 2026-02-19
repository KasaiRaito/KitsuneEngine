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
        case ColliderType2D::Square:
        {
            auto* b = (AABBColliderData*)data;
            const int x = (int)(transform->location.x() - b->halfWidth);
            const int y = (int)(transform->location.y() - b->halfHeight);
            const int w = (int)(b->halfWidth * 2.0f);
            const int h = (int)(b->halfHeight * 2.0f);
            DrawRectangleLines(x, y, w, h, GREEN);
        } break;

        default:
            break;
    }
}
