#include "Object.h"
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
}

void Object::Update(float dt) {
    if (velocity.Length() != 0)
    {
        transform.location.Translate(velocity * dt);
    }

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
