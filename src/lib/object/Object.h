#pragma once

#include "Transform2D.h"
#include "Vector2D.h"
#include "List.h"
#include "Collider2D.h"
#include "Component.h"
#include "Events.h"

class Object {
public:
    Transform2D transform;
    Vector2D velocity;
    Collider2D* collider = nullptr;
    List<Component*> components;
    List<Subscription> subscriptions;

    Object();
    virtual ~Object();

    void AddComponent(Component* component);

    template <typename Payload>
    void Subscribe(EventChannel<Payload>& channel, void (*fn)(void*, const Payload&))
    {
        subscriptions.Add(channel.SubscribeScoped(this, fn));
    }

    void UnsubscribeAll()
    {
        for (int i = 0; i < subscriptions.Size(); i++)
        {
            subscriptions[i].Unsubscribe();
        }

        subscriptions.ClearSubscriptions();
    }



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

    virtual void OnCollisionEnter(Object* other);
    virtual void OnCollisionStay(Object* other);
};
