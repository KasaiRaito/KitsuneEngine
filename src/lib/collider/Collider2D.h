#pragma once
#include "ColliderTypes2D.h"
#include "vector/Vector2D.h"
#include "raylib.h"
#include "Transform2D.h"

// Forward declare your data types if needed:
// struct CircleColliderData;
// struct AABBColliderData;

struct Collider2D {
    ColliderType2D type{};
    Transform2D* transform = nullptr; // pointer to owner's transform position
    void* data = nullptr;         // points to CircleColliderData or AABBColliderData
    bool isTrigger = false;

    void DebugDraw() const;
};
