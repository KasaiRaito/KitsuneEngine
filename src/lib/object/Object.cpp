#include "Object.h"

#include "ColliderComponent2D.h"
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

void Object::Update(float dt) {

    for (int i = 0; i < components.Size(); i++) {
        components[i]->Update(dt);
    }

    if (velocity.Length() != 0)
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
    velocity = Vector2D::Zero();
    other->velocity = Vector2D::Zero();
    DrawText("Colliding", 400, 20, 20, RED);

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
