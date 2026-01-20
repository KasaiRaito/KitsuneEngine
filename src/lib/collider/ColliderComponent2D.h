#ifndef KITSUNEENGINE_COLLIDER_COMPONENT_2D_H
#define KITSUNEENGINE_COLLIDER_COMPONENT_2D_H

#include "ColliderTypes2D.h"
#include "Transform2D.h"

struct ColliderComponent2D {
    ColliderShape2D shape = ColliderShape2D::None;
    Transform2D* transform = nullptr;

    CircleShape circle;
    SquareShape square;

    void SetCircle(float radius) {
        shape = ColliderShape2D::Circle;
        circle.radius = radius;
    }

    void SetSquare(float wth, float hth) {
        shape = ColliderShape2D::Square;
        square.width  = wth;
        square.height = hth;
    }
};

#endif
