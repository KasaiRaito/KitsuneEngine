#include "raylib.h"
#include "Debug.h"
#include "SceneManager.h"
#include "SceneMain.h"
#include "AngryBalls.h"
#include "DinoJump.h"
#include "ResourceManager.h"


#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main()
{
    InitWindow(800, 450, "Kitsune Engine");
    SetTargetFPS(120);

    Debug::SetDebug(true);

    SceneManager sceneManager;
    sceneManager.AddScene(SceneInfo(new SceneMain(&sceneManager), "main", 0));
    sceneManager.AddScene(SceneInfo(new AngryBalls(&sceneManager), "angryballs", 1));
    sceneManager.AddScene(SceneInfo(new DinoJump(&sceneManager), "dinojump", 2));
    sceneManager.LoadScene(0);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        SceneInfo current = sceneManager.GetCurrentScene();
        if (!current.scene) break;

        current.scene->Update(dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        current.scene->Draw();

        EndDrawing();
    }

    // Free all cached resources before closing the render/audio systems.
    ResourceManager::Instance().Clear();
    CloseWindow();
    return 0;
}
