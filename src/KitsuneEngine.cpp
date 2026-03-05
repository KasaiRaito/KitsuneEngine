#include "raylib.h"
#include "Debug.h"
#include "SceneManager.h"
#include "SceneMain.h"
#include "game/scene/GameLoadingScene.h"
#include "game/scene/AngryBalls/AngryBalls.h"
#include "game/scene/AngryBalls/AngryBalls_Level1.h"
#include "game/scene/AngryBalls/AngryBalls_Level2.h"
#include "game/scene/AngryBalls/AngryBalls_Level3.h"
#include "game/scene/AngryBalls/AngryBalls_Level4.h"
#include "game/scene/DinoJump/DinoJump.h"
#include "game/scene/SpaceInvaders/SpaceInvaders.h"
#include "game/scene/WhereIsMyWhater/WhereIsMyWater.h"
#include "game/scene/WhereIsMyWhater/WhereIsMyWater_Level1.h"
#include "game/scene/WhereIsMyWhater/WhereIsMyWater_Level2.h"
#include "game/scene/WhereIsMyWhater/WhereIsMyWater_Level3.h"
#include "game/scene/WhereIsMyWhater/WhereIsMyWater_Level4.h"
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
    sceneManager.AddScene(SceneInfo(new AngryBalls(&sceneManager), "angryballs_menu", 1));
    sceneManager.AddScene(SceneInfo(new DinoJump(&sceneManager), "dinojump", 2));
    sceneManager.AddScene(SceneInfo(new AngryBalls_Level1(&sceneManager), "angryballs_level1", 3));
    sceneManager.AddScene(SceneInfo(new AngryBalls_Level2(&sceneManager), "angryballs_level2", 4));
    sceneManager.AddScene(SceneInfo(new AngryBalls_Level3(&sceneManager), "angryballs_level3", 5));
    sceneManager.AddScene(SceneInfo(new AngryBalls_Level4(&sceneManager), "angryballs_level4", 6));
    sceneManager.AddScene(SceneInfo(new SpaceInvaders(&sceneManager), "space_invaders", 7));
    sceneManager.AddScene(SceneInfo(new WhereIsMyWater(&sceneManager), "where_is_my_water_menu", 8));
    sceneManager.AddScene(SceneInfo(new WhereIsMyWater_Level1(&sceneManager), "where_is_my_water_level1", 9));
    sceneManager.AddScene(SceneInfo(new WhereIsMyWater_Level2(&sceneManager), "where_is_my_water_level2", 10));
    sceneManager.AddScene(SceneInfo(new WhereIsMyWater_Level3(&sceneManager), "where_is_my_water_level3", 11));
    sceneManager.AddScene(SceneInfo(new WhereIsMyWater_Level4(&sceneManager), "where_is_my_water_level4", 12));

    sceneManager.AddScene(SceneInfo(
        new GameLoadingScene(
            &sceneManager,
            2,
            "Dino Jump",
            {
                "Space / Up / W: jump (hold for higher jump)",
                "R: restart after game over",
                "Avoid cacti and birds"
            }
        ),
        "loading_dinojump",
        13
    ));

    sceneManager.AddScene(SceneInfo(
        new GameLoadingScene(
            &sceneManager,
            7,
            "Space Invaders",
            {
                "A/D or Left/Right: move",
                "Space / Up / W: shoot",
                "R or Enter: restart after game over"
            }
        ),
        "loading_space_invaders",
        14
    ));

    sceneManager.AddScene(SceneInfo(
        new GameLoadingScene(
            &sceneManager,
            3,
            "Angry Balls - Level 1",
            {
                "Click and drag bird from slingshot",
                "Release mouse to launch",
                "R: restart level, ESC: back to level menu"
            }
        ),
        "loading_angryballs_level1",
        15
    ));

    sceneManager.AddScene(SceneInfo(
        new GameLoadingScene(
            &sceneManager,
            4,
            "Angry Balls - Level 2",
            {
                "Click and drag bird from slingshot",
                "Release mouse to launch",
                "R: restart level, ESC: back to level menu"
            }
        ),
        "loading_angryballs_level2",
        16
    ));

    sceneManager.AddScene(SceneInfo(
        new GameLoadingScene(
            &sceneManager,
            5,
            "Angry Balls - Level 3",
            {
                "Click and drag bird from slingshot",
                "Release mouse to launch",
                "R: restart level, ESC: back to level menu"
            }
        ),
        "loading_angryballs_level3",
        17
    ));

    sceneManager.AddScene(SceneInfo(
        new GameLoadingScene(
            &sceneManager,
            6,
            "Angry Balls - Level 4",
            {
                "Click and drag bird from slingshot",
                "Release mouse to launch",
                "R: restart level, ESC: back to level menu"
            }
        ),
        "loading_angryballs_level4",
        18
    ));

    sceneManager.AddScene(SceneInfo(
        new GameLoadingScene(
            &sceneManager,
            9,
            "Where Is My Water? - Level 1",
            {
                "Left Mouse: dig through dirt",
                "Any key: start water flow",
                "R: restart, ESC/L: level select"
            }
        ),
        "loading_where_is_my_water_level1",
        19
    ));

    sceneManager.AddScene(SceneInfo(
        new GameLoadingScene(
            &sceneManager,
            10,
            "Where Is My Water? - Level 2",
            {
                "Left Mouse: dig through dirt",
                "Any key: start water flow",
                "R: restart, ESC/L: level select"
            }
        ),
        "loading_where_is_my_water_level2",
        20
    ));

    sceneManager.AddScene(SceneInfo(
        new GameLoadingScene(
            &sceneManager,
            11,
            "Where Is My Water? - Level 3",
            {
                "Left Mouse: dig through dirt",
                "Any key: start water flow",
                "R: restart, ESC/L: level select"
            }
        ),
        "loading_where_is_my_water_level3",
        21
    ));

    sceneManager.AddScene(SceneInfo(
        new GameLoadingScene(
            &sceneManager,
            12,
            "Where Is My Water? - Level 4",
            {
                "Left Mouse: dig through dirt",
                "Any key: start water flow",
                "R: restart, ESC/L: level select"
            }
        ),
        "loading_where_is_my_water_level4",
        22
    ));

    sceneManager.LoadScene(0);

    while (!WindowShouldClose() && !sceneManager.IsQuitRequested())
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
