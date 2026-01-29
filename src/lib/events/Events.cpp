//
// Created by Eduardo Huerta on 29/01/26.
//

#include "Events.h"
#include "Object.h"

Events GEvents;

static void OnAnyCollision(void* ctx, const CollisionEvent& e)
{
    Object* self = (Object*)ctx;

    // If this object is involved
    if (e.a == self || e.b == self)
    {
        // Do something
        // TraceLog(LOG_INFO, "Hit!");
    }
}

static void OnCollisionEnter(void* ctx, const CollisionEvent& e) {
    Object* self = (Object*)ctx;
    Object* a = (Object*)e.a;
    Object* b = (Object*)e.b;

    if (self == a || self == b)
        self->OnCollisionEnter(self == a ? b : a);
}

static void OnMovementToggle(void* ctx, const MovementToggleEvent& e) {
    Object* self = (Object*)ctx;
    void* who = e.who;
    bool enabled = e.enabled;
}

void RegisterBall(Object* ball)
{
    GEvents.OnCollisionEnter.Subscribe(ball, &OnAnyCollision);
}
