#pragma once
#include "ColliderTypes2D.h"
#include "vector/Vector2D.h"
#include "raylib.h"

// Forward declare your data types if needed:
// struct CircleColliderData;
// struct AABBColliderData;

struct Collider2D {
    ColliderType2D type{};
    Vector2D* position = nullptr; // pointer to owner's transform position
    void* data = nullptr;         // points to CircleColliderData or AABBColliderData
    bool isTrigger = false;

    void DebugDraw() const;
};
