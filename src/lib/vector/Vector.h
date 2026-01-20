#ifndef KITSUNEENGINE_VECTOR_H
#define KITSUNEENGINE_VECTOR_H

#include "Vector2D.h"
#include "Vector3D.h"

namespace Math {

    // Vector3
    Vector3D Add(const Vector3D& a, const Vector3D& b);
    Vector3D Subtract(const Vector3D& a, const Vector3D& b);

    // Vector2
    Vector2D Add(const Vector2D& a, const Vector2D& b);
    Vector2D Subtract(const Vector2D& a, const Vector2D& b);

}

#endif // KITSUNEENGINE_VECTOR_H
