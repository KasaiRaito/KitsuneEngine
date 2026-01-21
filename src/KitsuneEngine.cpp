#include "raylib.h"
#include "Ball.h"
#include "InputComponent.h"
#include "CircleRenderComponent.h"
#include "ColliderComponent2D.h"
#include "Debug.h"
#include "Scene.h"

int main()
{
    Scene scene;

    Debug::SetDebug(true);

    InitWindow(800, 450, "Kitsune Engine");
    SetTargetFPS(60);

    Ball* ball = new Ball();
    ball->transform.location.value = {400, 225};

    auto* render = new CircleRenderComponent();
    render->radius = 30.0f;
    render->color = RED;

    auto* collider = new ColliderComponent2D(ColliderType2D::Circle, render);

    ball->AddComponent(render);
    ball->AddComponent(collider);

    scene.AddObject(ball);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        scene.Update(dt);     // ✅ update via scene, not ball.Update(dt)

        BeginDrawing();
        ClearBackground(RAYWHITE);

        scene.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0; // ✅ Scene destructor will delete ball ONCE
}