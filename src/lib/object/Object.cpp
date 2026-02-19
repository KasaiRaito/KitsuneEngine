#include "Object.h"

#include "ColliderComponent2D.h"
#include "physics_system/PhysicsComponent.h"
#include "physics_system/PhysicsSystem.h"
#include "RenderComponent2D.h"
#include "Events.h"

Object::Object() {
    
}

Object::~Object() {
    for (int i = 0; i < components.Size(); i++) {
        delete components[i];
    }

    //GEvents.OnCollisionEnter.Unsubscribe(this, &OnAnyCollision);

    UnsubscribeAll();

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

void Object::RemoveComponent(Component* component)
{
    if (!component) return;

    for (size_t i = 0; i < components.Size(); i++)
    {
        if (components[i] != component) continue;

        if (auto temp = dynamic_cast<ColliderComponent2D*>(component))
        {
            if (collider == temp->GetCollider())
                collider = nullptr;
        }

        component->OnRemoved();
        delete component;
        components.RemoveAt(i);
        return;
    }
}

void Object::Update(float dt) {

    for (int i = 0; i < components.Size(); i++) {
        components[i]->Update(dt);
    }

    if (auto* physics = GetComponent<PhysicsComponent>())
    {
        PhysicsSystem::Simulate(*this, *physics, dt);
    }
    else if (velocity.Length() != 0)
    {
        transform.location.Translate(velocity * dt);
    }

    if (components.Size() == 0)
        return;
}

void Object::Draw()
{
    for (int i = 0; i < components.Size(); i++) {
        if (auto renderer =
            dynamic_cast<RenderComponent2D*>(components[i])) {
            renderer->Draw();
            }
    }
}

void Object::OnCollisionEnter(Object *other) {

}

void Object::OnCollisionStay(Object *other) {
    (void)other;
}

void Object::OnCollisionExit(Object *other) { // CHANGED
    (void)other; // CHANGED
} // CHANGED

void Object::OnTriggerEnter(Object *other) { // CHANGED
    (void)other; // CHANGED
} // CHANGED

void Object::OnTriggerStay(Object *other) { // CHANGED
    (void)other; // CHANGED
} // CHANGED

void Object::OnTriggerExit(Object *other) { // CHANGED
    (void)other; // CHANGED
} // CHANGED
