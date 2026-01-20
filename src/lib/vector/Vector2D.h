#ifndef KITSUNEENGINE_VECTOR2D_H
#define KITSUNEENGINE_VECTOR2D_H


#include <cmath>

struct Vector2D {
    float x;
    float y;

    // Constructors
    constexpr Vector2D() : x(0), y(0) {}
    constexpr Vector2D(float x, float y) : x(x), y(y) {}
    static constexpr Vector2D Zero() { return {}; }

    // Length / magnitude
    float Length() const {
        return std::sqrt(x * x + y * y);
    }

    // Dot product
    float Dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }

    bool IsZero() const
    {
        return (x == 0 && y == 0);
    }

    // Normalization
    Vector2D Normalize() const {
        float len = Length();
        return (len > 0.0f) ? (*this / len) : Vector2D();
    }

    // Operators
    Vector2D operator+(const Vector2D& rhs) const {
        return Vector2D(x + rhs.x, y + rhs.y);
    }

    Vector2D operator-(const Vector2D& rhs) const {
        return Vector2D(x - rhs.x, y - rhs.y);
    }

    Vector2D& operator=(const Vector2D& rhs) {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }


    Vector2D operator*(float scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }

    Vector2D operator/(float scalar) const {
        return Vector2D(x / scalar, y / scalar);
    }

    Vector2D& operator+=(const Vector2D& rhs) {
        x += rhs.x; y += rhs.y;
        return *this;
    }

    Vector2D& operator-=(const Vector2D& rhs) {
        x -= rhs.x; y -= rhs.y;
        return *this;
    }

    Vector2D& VectorCero() {
        x = 0;
        y = 0;
        return *this;
    }
};


struct Vector2Di {
    int x;
    int y;

    // Constructors
    constexpr Vector2Di() : x(0), y(0) {}
    constexpr Vector2Di(float x, float y) : x(x), y(y) {}
    static constexpr Vector2Di Zero() { return {}; }

    // Length / magnitude
    float Length() const {
        return std::sqrt(x * x + y * y);
    }

    // Dot product
    float Dot(const Vector2Di& other) const {
        return x * other.x + y * other.y;
    }

    bool IsZero() const
    {
        return (x == 0 && y == 0);
    }

    // Normalization
    Vector2Di Normalize() const {
        float len = Length();
        return (len > 0.0f) ? (*this / len) : Vector2Di();
    }

    // Operators
    Vector2Di operator+(const Vector2Di& rhs) const {
        return Vector2Di(x + rhs.x, y + rhs.y);
    }

    Vector2Di operator-(const Vector2Di& rhs) const {
        return Vector2Di(x - rhs.x, y - rhs.y);
    }

    Vector2Di& operator=(const Vector2Di& rhs) {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }


    Vector2Di operator*(float scalar) const {
        return Vector2Di(x * scalar, y * scalar);
    }

    Vector2Di operator/(float scalar) const {
        return Vector2Di(x / scalar, y / scalar);
    }

    Vector2Di& operator+=(const Vector2Di& rhs) {
        x += rhs.x; y += rhs.y;
        return *this;
    }

    Vector2Di& operator-=(const Vector2Di& rhs) {
        x -= rhs.x; y -= rhs.y;
        return *this;
    }

    Vector2Di& VectorCero() {
        x = 0;
        y = 0;
        return *this;
    }
};
#endif //KITSUNEENGINE_VECTOR2D_H