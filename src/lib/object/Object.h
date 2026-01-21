#pragma once

#include "Transform2D.h"
#include "Vector2D.h"
#include "List.h"
#include "Collider2D.h"
#include "Component.h"

class Object {
public:
    Transform2D transform;
    Vector2D velocity;
    Collider2D* collider = nullptr;
    List<Component*> components;

    Object() = default;
    virtual ~Object();

    void AddComponent(Component* component);

    virtual void Update(float dt);

    void Draw();

    template<typename T>
    T* GetComponent() {
        for (int i = 0; i < components.Size(); i++) {
            if (auto casted = dynamic_cast<T*>(components[i])) {
                return casted;
            }
        }
        return nullptr;
    }
};
