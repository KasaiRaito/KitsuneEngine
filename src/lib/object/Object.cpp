#include "Object.h"

#include "ColliderComponent2D.h"
#include "RenderComponent2D.h"

Object::~Object() {
    for (int i = 0; i < components.Size(); i++) {
        delete components[i];
    }
    components.Clear();
}

void Object::AddComponent(Component* component) {
    component->owner = this;
    components.Add(component);

    if (auto temp = dynamic_cast<ColliderComponent2D *>(component))
    {
            collider = temp->GetCollider();
    }

    component->OnAdded();
}

void Object::Update(float dt) {
    if (velocity.Length() != 0)
    {
        transform.location.Translate(velocity * dt);
    }

    if (components.Size() == 0)
        return;

    for (int i = 0; i < components.Size(); i++) {
        components[i]->Update(dt);
    }
}

void Object::Draw() {
    for (int i = 0; i < components.Size(); i++) {
        if (auto renderer =
            dynamic_cast<RenderComponent2D*>(components[i])) {
            renderer->Draw();
            }
    }
}
