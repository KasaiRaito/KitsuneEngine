#ifndef KITSUNEENGINE_TRANSFORM2D_H
#define KITSUNEENGINE_TRANSFORM2D_H

#include "Vector2D.h"
struct Location2D {
    Vector2D value;

    constexpr Location2D() : value(Vector2D::Zero()) {}
    constexpr Location2D(const Vector2D& v) : value(v) {}

    static constexpr Location2D Zero() {
        return Location2D(Vector2D::Zero());
    }
    void Translate(const Vector2D& dt) {
        value += dt;
    }
};

struct Rotation2D {
    float angle; // radians (much better than Vector2D)

    constexpr Rotation2D() : angle(0.0f) {}
    constexpr Rotation2D(float a) : angle(a) {}

    void Rotate(float dt) {
        angle += dt;
    }
};

struct Scale2D {
    Vector2D value;

    constexpr Scale2D() : value(1.0f, 1.0f) {}
    constexpr Scale2D(const Vector2D& v) : value(v) {}
};

struct Transform2D {
    Location2D location;
    Rotation2D rotation;
    Scale2D scale;

    constexpr Transform2D()
        : location(),
          rotation(),
          scale() {}

    constexpr Transform2D(
        const Vector2D& pos,
        float rot,
        const Vector2D& scl)
        : location(pos),
          rotation(rot),
          scale(scl) {}

    Vector2D& Position() { return location.value; }
    const Vector2D& Position() const { return location.value; }

};

#endif //KITSUNEENGINE_TRANSFORM2D_H