#ifndef KITSUNEENGINE_VECTOR3D_H
#define KITSUNEENGINE_VECTOR3D_H

#include <cmath>

struct Vector3D {
    float x;
    float y;
    float z;

    // Constructors
    constexpr Vector3D() : x(0), y(0), z(0) {}
    constexpr Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}
    static constexpr Vector3D Zero() { return {}; }



    // Length / magnitude
    float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Dot product
    float Dot(const Vector3D& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Normalization
    Vector3D Normalized() const {
        float len = Length();
        return (len > 0.0f) ? (*this / len) : Vector3D();
    }

    // Operators
    Vector3D operator+(const Vector3D& rhs) const {
        return Vector3D(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vector3D operator-(const Vector3D& rhs) const {
        return Vector3D(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vector3D& operator=(const Vector3D& rhs) {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }

    Vector3D operator*(float scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    Vector3D operator/(float scalar) const {
        return Vector3D(x / scalar, y / scalar, z / scalar);
    }

    Vector3D& operator+=(const Vector3D& rhs) {
        x += rhs.x; y += rhs.y; z += rhs.z;
        return *this;
    }

    Vector3D& operator-=(const Vector3D& rhs) {
        x -= rhs.x; y -= rhs.y; z -= rhs.z;
        return *this;
    }
};

#endif //KITSUNEENGINE_VECTOR3D_H