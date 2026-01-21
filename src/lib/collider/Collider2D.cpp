#include "Collider2D.h"

void Collider2D::DebugDraw() const
{
    if (!position || !data) return;

    switch (type)
    {
        case ColliderType2D::Circle:
        {
            auto* c = (CircleColliderData*)data;
            DrawCircleLines((int)position->x, (int)position->y, c->radius, GREEN);
        } break;

        default:
            break;
    }
}
