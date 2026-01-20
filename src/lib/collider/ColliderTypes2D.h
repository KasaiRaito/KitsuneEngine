#ifndef KITSUNEENGINE_COLLIDER_TYPES_2D_H
#define KITSUNEENGINE_COLLIDER_TYPES_2D_H

enum class ColliderShape2D {
    None = 0,
    Circle,
    Square,
};

struct CircleShape {
    float radius = 0.0f;
};

struct SquareShape {
    float width  = 0.0f;
    float height = 0.0f;
};

#endif
