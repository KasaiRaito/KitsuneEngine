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

SceneGame::SceneGame()
{
    Ball* Player = new Ball();
    Player->transform.location.value = { 200, 225 };

    auto* render = new CircleRenderComponent();
    render->radius = 25.0f;
    render->color = BLUE;

    auto* collider = new ColliderComponent2D(ColliderType2D::Circle, render);

    Player->AddComponent(render);
    Player->AddComponent(collider);

    auto* input = new InputComponent();
    Player->AddComponent(input);

    // Move axis
    auto* move = input->AddAxis2D("Move");
    move->up.Add(KEY_W);
    move->left.Add(KEY_A);
    move->down.Add(KEY_S);
    move->right.Add(KEY_D);

    // Interact action
    auto* interact = input->AddAction("Interact");
    interact->keys.Add(KEY_E);

    auto* toggleMovement = input->AddAction("Toggle Movement");
    toggleMovement->keys.Add(KEY_P);

    objects.Add(Player);

    Ball* ball = new Ball();
    ball->transform.location.value = { 300, 300 };
    auto* render2 = new CircleRenderComponent();
    render2->radius = 25.0f;
    render2->color = BLUE;

    auto* collider2 = new ColliderComponent2D(ColliderType2D::Circle, render);

    ball->AddComponent(render2);
    ball->AddComponent(collider2);

    objects.Add(ball);

    SetTime(00,00,03,00);
}


SceneGame::~SceneGame()
{
    for (size_t i = 0; i < objects.Size(); i++)
        delete objects.Get(i);
}

void SceneGame::Update(float dt)
{
    UpdateClock(dt, false);

    // Now update objects
    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj) continue;
        obj->Update(dt);
    }

    // Collision pass (no extra updates here)
    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* a = objects.Get(i);
        if (!a || !a->collider) continue;

        for (size_t j = i + 1; j < objects.Size(); j++)
        {
            Object* b = objects.Get(j);
            if (!b || !b->collider) continue;

            auto m = CollisionSystem2D::Test(*a->collider, *b->collider);

            ColliderComponent2D* ca = a->GetComponent<ColliderComponent2D>();
            ColliderComponent2D* cb = b->GetComponent<ColliderComponent2D>();

            if (!ca || !cb) continue;

            bool wasColliding = (ca->collidingWith.count(b) > 0);

            if (m.colliding)
            {
                if (!wasColliding)
                {
                    ca->collidingWith.insert(b);
                    cb->collidingWith.insert(a);

                    ca->OnCollisionEnter(b);
                    cb->OnCollisionEnter(a);
                }
                else
                {
                    ca->OnCollisionStay(b);
                    cb->OnCollisionStay(a);
                }
            }
            else
            {
                if (wasColliding)
                {
                    ca->collidingWith.erase(b);
                    cb->collidingWith.erase(a);

                    ca->OnCollisionExit(b);
                    cb->OnCollisionExit(a);
                }
            }

        }
    }
}

void SceneGame::Draw()
{
    int h, m, s, ms;
    GetClockParts(h, m, s, ms);

    std::ostringstream clockTXT;
    clockTXT << std::setfill('0')
             << std::setw(2) << h << ":"
             << std::setw(2) << m << ":"
             << std::setw(2) << s << ":"
             << std::setw(3) << ms;

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
    // clamp negatives
    if (h < 0) h = 0;
    if (m < 0) m = 0;
    if (s < 0) s = 0;
    if (ms < 0) ms = 0;

    // optional: clamp max ranges
    if (h > 23) h = 0;
    if (m > 59) m = 59;
    if (s > 59) s = 59;
    if (ms > 999) ms = 999;

    totalMs = (h * 60 * 60 * 1000)
            + (m * 60 * 1000)
            + (s * 1000)
            + ms;
}

void SceneGame::UpdateClock(float dt, bool grow)
{
    int deltaMs = (int)(dt * 1000.0f);

    if (grow)
        totalMs += deltaMs;
    else
        totalMs -= deltaMs;

    // clamp at 0 (no negative time)
    if (totalMs < 0)
        totalMs = 0;

    if (totalMs >= dayMs)
    {
        totalMs %= dayMs;
    }

    // optional: wrap at 24 hours (uncomment if you want)
    // int dayMs = 24 * 60 * 60 * 1000;
    // totalMs %= dayMs;
}

void SceneGame::GetClockParts(int& h, int& m, int& s, int& ms) const
{
    int t = totalMs;

    ms = t % 1000;
    t /= 1000;

    s = t % 60;
    t /= 60;

    m = t % 60;
    t /= 60;

    h = t; // if you want wrap 24h: h %= 24;
}