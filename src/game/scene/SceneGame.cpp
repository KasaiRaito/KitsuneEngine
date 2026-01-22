#include "SceneGame.h"

#include "Ball.h"
#include "CircleRenderComponent.h"
#include "ColliderComponent2D.h"
#include "CollisionSystem2D.h"
#include "InputComponent.h"
#include "Debug.h"
#include "raylib.h"

#include <sstream>
#include <iomanip>


// 1) Callbacks (top of file)
static void MoveUp(void* ctx, float dt)
{
    Object* o = (Object*)ctx;
    o->transform.location.value.y -= 200.0f * dt;
}

static void MoveDown(void* ctx, float dt)
{
    Object* o = (Object*)ctx;
    o->transform.location.value.y += 200.0f * dt;
}

static void MoveLeft(void* ctx, float dt)
{
    Object* o = (Object*)ctx;
    o->transform.location.value.x -= 200.0f * dt;
}

static void MoveRight(void* ctx, float dt)
{
    Object* o = (Object*)ctx;
    o->transform.location.value.x += 200.0f * dt;
}

// 2) Scene constructor

SceneGame::SceneGame()
{
    Ball* ball = new Ball();
    ball->transform.location.value = { 200, 225 };

    auto* render = new CircleRenderComponent();
    render->radius = 25.0f;
    render->color = BLUE;

    auto* collider = new ColliderComponent2D(ColliderType2D::Circle, render);

    ball->AddComponent(render);
    ball->AddComponent(collider);

    auto* input = new InputComponent();
    ball->AddComponent(input);

    // Move axis
    auto* move = input->AddAxis2D("Move");
    move->up.Add(KEY_W);
    move->left.Add(KEY_A);
    move->down.Add(KEY_S);
    move->right.Add(KEY_D);

    // Interact action
    auto* interact = input->AddAction("Interact");
    interact->keys.Add(KEY_E);

    objects.Add(ball);
}


SceneGame::~SceneGame()
{
    for (size_t i = 0; i < objects.Size(); i++)
        delete objects.Get(i);
}

void SceneGame::Update(float dt)
{
    UpdateClock(dt);

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

            if (!b) continue;

            b->Update(dt);
        }
    }
}

void SceneGame::Draw()
{
    std::ostringstream clockTXT;
    clockTXT << std::setfill('0')
       << std::setw(2) << hours << ":"
       << std::setw(2) << minutes << ":"
       << std::setw(2) << seconds << ":"
       << std::setw(3) << milliseconds;
    DrawText(clockTXT.str().c_str(), 20, 100, 20, DARKBLUE);

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

void SceneGame::SetTime(int h, int m, int s, int ms)
{
    hours = h;
    minutes = m;
    seconds = s;
    milliseconds = ms;
}
void SceneGame::UpdateClock(float dt)
{
    milliseconds += (int)(dt * 1000.0f);

    while (milliseconds >= 1000) {
        milliseconds -= 1000;
        seconds++;
    }
    while (seconds >= 60) {
        seconds -= 60;
        minutes++;
    }
    while (minutes >= 60) {
        minutes -= 60;
        hours++;
    }
    while (hours >= 24) {
        hours -= 24;
    }
}
