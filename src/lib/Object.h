#ifndef KITSUNEENGINE_OBJECT_H
#define KITSUNEENGINE_OBJECT_H

#include "Transform2D.h"
#include "Vector2D.h"
#include "ColliderComponent2D.h"

class Object {
public:
    Transform2D transform;
    Vector2D velocity;

    ColliderComponent2D collider;

    Object() {
        collider.transform = &transform;
    }

    virtual ~Object() = default;
};

#endif
