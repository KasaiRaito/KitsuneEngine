#include "raylib.h"
#include "Balls.h"
#include "List.h"

int main() {
    Vector2D windowSize(800.0f, 450.0f);

    InitWindow(
        static_cast<int>(windowSize.x),
        static_cast<int>(windowSize.y),
        "Kitsune Engine"
    );

    SetTargetFPS(60);

    Ball ball = Ball(Vector2D(windowSize.x/2.0f,windowSize.y/2.0f),5.0f);

    ball.transform.location.value = {
        windowSize.x * 0.5f,
        windowSize.y * 0.5f
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        ball.Update(dt);

        //Get All children from Object.h /cpp (Balls) and call their begin draw()
        BeginDrawing();
        ClearBackground(RAYWHITE);

        ball.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
