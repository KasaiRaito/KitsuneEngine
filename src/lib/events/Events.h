//
// Created by Eduardo Huerta on 29/01/26.
//

#ifndef KITSUNEENGINE_EVENTS_H
#define KITSUNEENGINE_EVENTS_H

// Events.h
#pragma once
#include "EventManager.h"
#include "vector/Vector2D.h"



struct MovementToggleEvent
{
    void* who = nullptr;   // Object* (nullptr = everyone)
    bool enabled = true;   // false = lock movement
};

struct CollisionEvent
{
    void* a;         // Object* (use void* to avoid include)
    void* b;         // Object*
    Vector2D normal;
    float penetration = 0.0f;
};

struct InteractEvent
{
    void* who;       // Object*
};

struct Events
{
    EventChannel<CollisionEvent> OnCollisionEnter;
    EventChannel<InteractEvent>  OnInteract;

    EventChannel<MovementToggleEvent> OnMovementToggle; // NEW
};

// global-ish singleton (or store in your Game/SceneManager)
extern Events GEvents;

#endif //KITSUNEENGINE_EVENTS_H