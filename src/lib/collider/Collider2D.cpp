#include "Collider2D.h"

#include <cmath>

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
            const Vector2 center = { transform->location.x(), transform->location.y() };
            const float angle = transform->rotation.angle;

            auto rotate = [center, angle](Vector2 point) -> Vector2
            {
                const float c = std::cos(angle);
                const float s = std::sin(angle);
                const float x = point.x - center.x;
                const float y = point.y - center.y;
                return {
                    center.x + x * c - y * s,
                    center.y + x * s + y * c
                };
            };

            Vector2 corners[4] = {
                rotate({ center.x - b->halfWidth, center.y - b->halfHeight }),
                rotate({ center.x + b->halfWidth, center.y - b->halfHeight }),
                rotate({ center.x + b->halfWidth, center.y + b->halfHeight }),
                rotate({ center.x - b->halfWidth, center.y + b->halfHeight })
            };

            DrawRectanglePro(
                { center.x, center.y, b->halfWidth * 2.0f, b->halfHeight * 2.0f },
                { b->halfWidth, b->halfHeight },
                transform->rotation.angle * RAD2DEG,
                ColorAlpha(GREEN, 0.25f)
            );
            DrawLineEx(corners[0], corners[1], 1.0f, GREEN);
            DrawLineEx(corners[1], corners[2], 1.0f, GREEN);
            DrawLineEx(corners[2], corners[3], 1.0f, GREEN);
            DrawLineEx(corners[3], corners[0], 1.0f, GREEN);
        } break;

        default:
            break;
    }
}
