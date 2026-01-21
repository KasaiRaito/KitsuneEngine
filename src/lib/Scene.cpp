#include "Scene.h"
#include "CollisionSystem2D.h"
#include "InputSystem.h"
#include "debug/Debug.h"

Scene::Scene() {}

Scene::~Scene() {
    for (size_t i = 0; i < objects.Size(); i++) {
        delete (Object*)objects.Get(i);
    }
}

void Scene::AddObject(Object* obj) {
    objects.Add(obj);
}

void Scene::DeleteObjects() {
    for (size_t i = 0; i < objects.Size(); i++) {
        delete (Object*)objects.Get(i);
    }
    objects.Clear(); // important
}

void Scene::Update(float dt) {
    InputSystem::ProcessInput();

    for (size_t i = 0; i < objects.Size(); i++) {
        Object* obj = (Object*)objects.Get(i);
        obj->Update(dt);
    }

    for (size_t i = 0; i < objects.Size(); i++) {
        Object* a = (Object*)objects.Get(i);
        if (!a->collider) continue;

        for (size_t j = i + 1; j < objects.Size(); j++) {
            Object* b = (Object*)objects.Get(j);
            if (!b->collider) continue;

            auto result = CollisionSystem2D::Test(*a->collider, *b->collider);
            if (result.colliding) {
                // later: resolve using manifold
            }
        }
    }
}

void Scene::Draw()
{
    for (size_t i = 0; i < objects.Size(); i++)
        ((Object*)objects.Get(i))->Draw();

    if (!Debug::GetDebug()) return;

    DrawText("DEBUG COLLIDER OVERLAY ACTIVE", 20, 20, 20, RED);

    for (size_t j = 0; j < objects.Size(); j++) {
        Object* obj = (Object*)objects.Get(j);
        if (!obj->collider) continue;
        obj->collider->DebugDraw();
    }
}