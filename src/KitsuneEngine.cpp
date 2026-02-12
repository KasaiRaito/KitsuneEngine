#include "raylib.h"
#include "Debug.h"
#include "SceneManager.h"
#include "SceneMain.h"
#include "SceneGame.h"
#include "lib/resource_manager/ResourceManager.h"

int main()
{
    InitWindow(800, 450, "Kitsune Engine");
    SetTargetFPS(60);

    Debug::SetDebug(true);

    SceneManager sceneManager;
    sceneManager.AddScene(SceneInfo(new SceneMain(), "main", 0));
    sceneManager.AddScene(SceneInfo(new SceneGame(), "game", 1));
    sceneManager.LoadScene(0);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_SPACE))
            sceneManager.NextScene();

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
