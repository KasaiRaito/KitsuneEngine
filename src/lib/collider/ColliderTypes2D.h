#pragma once

enum class ColliderType2D {
    Circle,
    Square,
};

struct CircleColliderData {
    float radius;
};

struct AABBColliderData {
    float halfWidth;
    float halfHeight;
};
