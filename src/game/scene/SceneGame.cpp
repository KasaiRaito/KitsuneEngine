#include "SceneGame.h"

#include "Ball.h"
#include "CircleRenderComponent.h"
#include "ColliderComponent2D.h"
#include "CollisionSystem2D.h"
#include "InputSystem.h"
#include "Debug.h"
#include "raylib.h"

SceneGame::SceneGame()
{
    // Example: different starting position / color so you can tell scenes apart
    Ball* ball = new Ball();
    ball->transform.location.value = {200, 225};

    auto* render = new CircleRenderComponent();
    render->radius = 25.0f;
    render->color = BLUE;

    auto* collider = new ColliderComponent2D(ColliderType2D::Circle, render);

    ball->AddComponent(render);
    ball->AddComponent(collider);

    objects.Add(ball);
}

SceneGame::~SceneGame()
{
    for (size_t i = 0; i < objects.Size(); i++)
        delete objects.Get(i);
}

void SceneGame::Update(float dt)
{
    InputSystem::ProcessInput();

    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj) continue;
        obj->Update(dt);
    }

    // Optional collision pass (copy/paste from SceneMain if you want it here too)
    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* a = objects.Get(i);
        if (!a || !a->collider) continue;

        for (size_t j = i + 1; j < objects.Size(); j++)
        {
            Object* b = objects.Get(j);
            if (!b || !b->collider) continue;

            auto result = CollisionSystem2D::Test(*a->collider, *b->collider);
            (void)result;
        }
    }
}

void SceneGame::Draw()
{
    DrawText("SceneGame (SPACE to switch)", 20, 20, 20, DARKBLUE);

    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj) continue;
        obj->Draw();
    }

    if (!Debug::GetDebug()) return;

    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj || !obj->collider) continue;
        obj->collider->DebugDraw();
    }
}
