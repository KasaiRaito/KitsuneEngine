#include "SceneMain.h"
#include "Ball.h"
#include "CircleRenderComponent.h"
#include "ColliderComponent2D.h"
#include "CollisionSystem2D.h"
#include "Debug.h"
#include "InputSystem.h"
#include "raylib.h"

SceneMain::SceneMain()
{
    Ball* ball = new Ball();
    ball->transform.location.value = {400, 225};

    auto* render = new CircleRenderComponent();
    render->radius = 30.0f;
    render->color = RED;

    auto* collider = new ColliderComponent2D(ColliderType2D::Circle, render);

    ball->AddComponent(render);
    ball->AddComponent(collider);

    objects.Add(ball); // objects should be List<Object*>
}

SceneMain::~SceneMain()
{
    for (size_t i = 0; i < objects.Size(); i++)
        delete objects.Get(i);
}

void SceneMain::Update(float dt)
{
    // Input once per frame
    InputSystem::ProcessInput();

    // Update objects
    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj) continue;
        obj->Update(dt);
    }

    // Collision pass
    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* a = objects.Get(i);
        if (!a || !a->collider) continue;

        for (size_t j = i + 1; j < objects.Size(); j++)
        {
            Object* b = objects.Get(j);
            if (!b || !b->collider) continue;

            auto result = CollisionSystem2D::Test(*a->collider, *b->collider);
            if (result.colliding)
            {
                // later: resolve using manifold
            }
        }
    }
}

void SceneMain::Draw()
{
    // If SceneBase::Draw() is abstract/empty, you can remove this call.
    SceneBase::Draw();

    // Draw objects
    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj) continue;
        obj->Draw();
    }

    // Debug overlay
    if (!Debug::GetDebug()) return;

    DrawText("SceneMenu (SPACE to switch)", 20, 20, 20, RED);

    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj || !obj->collider) continue;

        obj->collider->DebugDraw();
    }
}
