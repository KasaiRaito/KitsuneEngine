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

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

namespace
{
    std::vector<std::string> startupLogLines;
    bool captureStartupLogs = false;
    constexpr size_t kMaxStartupLogLines = 220;

    const char* ToLogLevelLabel(int logLevel)
    {
        switch (logLevel)
        {
        case LOG_TRACE: return "TRACE";
        case LOG_DEBUG: return "DEBUG";
        case LOG_INFO: return "INFO";
        case LOG_WARNING: return "WARNING";
        case LOG_ERROR: return "ERROR";
        case LOG_FATAL: return "FATAL";
        default: return "LOG";
        }
    }

    void StartupTraceLogCallback(int logLevel, const char* text, va_list args)
    {
        char messageBuffer[2048];
        std::vsnprintf(messageBuffer, sizeof(messageBuffer), text, args);

        char fullLogLine[2200];
        std::snprintf(
            fullLogLine,
            sizeof(fullLogLine),
            "%s: %s",
            ToLogLevelLabel(logLevel),
            messageBuffer
        );

        std::fputs(fullLogLine, stdout);
        std::fputc('\n', stdout);
        std::fflush(stdout);

        if (!captureStartupLogs)
            return;

        const bool shouldCapture = (logLevel == LOG_INFO) || (logLevel >= LOG_WARNING);
        if (!shouldCapture)
            return;

        startupLogLines.emplace_back(fullLogLine);
        if (startupLogLines.size() > kMaxStartupLogLines)
            startupLogLines.erase(startupLogLines.begin());
    }

    void DrawStartupLoader(size_t completedSteps, size_t totalSteps, const std::string& currentStep)
    {
        const int screenW = GetScreenWidth();
        const int screenH = GetScreenHeight();
        const bool finished = completedSteps >= totalSteps;
        const float progress = (totalSteps == 0)
            ? 1.0f
            : (float)completedSteps / (float)totalSteps;

        DrawRectangleGradientV(
            0,
            0,
            screenW,
            screenH,
            Color{ 24, 32, 48, 255 },
            Color{ 9, 14, 23, 255 }
        );

        DrawText("Kitsune Engine", 20, 18, 48, RAYWHITE);
        DrawText("Startup Loader", 22, 70, 25, Color{ 189, 222, 245, 255 });

        const Rectangle progressBackground = { 20.0f, 108.0f, (float)screenW - 40.0f, 22.0f };
        DrawRectangleRounded(progressBackground, 0.25f, 8, Fade(WHITE, 0.17f));

        Rectangle progressBar = progressBackground;
        progressBar.width *= std::clamp(progress, 0.0f, 1.0f);
        if (progressBar.width > 1.0f)
            DrawRectangleRounded(progressBar, 0.25f, 8, Color{ 114, 220, 255, 255 });

        const char* statusText = finished
            ? "Startup completed successfully."
            : TextFormat("Loading resources and scenes... %d/%d", (int)completedSteps, (int)totalSteps);
        DrawText(statusText, 20, 140, 21, RAYWHITE);

        DrawText(
            TextFormat("Current: %s", currentStep.c_str()),
            20,
            166,
            19,
            Color{ 201, 211, 228, 255 }
        );

        const Rectangle logPanel = { 20.0f, 196.0f, (float)screenW - 40.0f, (float)screenH - 238.0f };
        DrawRectangleRounded(logPanel, 0.06f, 8, Fade(BLACK, 0.45f));
        DrawRectangleRoundedLinesEx(logPanel, 0.06f, 8, 2.0f, Fade(WHITE, 0.24f));
        DrawText("Runtime logs", (int)logPanel.x + 12, (int)logPanel.y + 8, 20, Color{ 183, 231, 255, 255 });

        const int fontSize = 16;
        const int lineHeight = 18;
        const int maxVisibleLines = std::max(1, (int)((logPanel.height - 40.0f) / (float)lineHeight));
        const int firstLineIndex = std::max(0, (int)startupLogLines.size() - maxVisibleLines);

        int lineY = (int)logPanel.y + 34;
        if (startupLogLines.empty())
        {
            DrawText("Waiting for loader output...", (int)logPanel.x + 12, lineY, fontSize, LIGHTGRAY);
        }
        else
        {
            for (int i = firstLineIndex; i < (int)startupLogLines.size(); ++i)
            {
                const char* clippedLine = TextSubtext(startupLogLines[(size_t)i].c_str(), 0, 116);
                DrawText(clippedLine, (int)logPanel.x + 12, lineY, fontSize, LIGHTGRAY);
                lineY += lineHeight;
            }
        }

        const char* hintText = finished
            ? "Press \"Space\" or \"Enter\" to continue."
            : "Please wait, loading game data...";
        const int hintWidth = MeasureText(hintText, 20);
        DrawText(hintText, (screenW - hintWidth) / 2, screenH - 28, 20, Color{ 196, 205, 216, 255 });
    }
}

int main()
{
    InitWindow(800, 450, "Kitsune Engine");
    SetTargetFPS(120);
    SetTraceLogLevel(LOG_INFO);
    SetTraceLogCallback(StartupTraceLogCallback);

    Debug::SetDebug(true);

    SceneManager sceneManager;
    using LoadingStep = std::pair<std::string, std::function<void()>>;
    std::vector<LoadingStep> loadingSteps;
    loadingSteps.reserve(23);

    auto AddLoadingStep = [&loadingSteps](std::string label, std::function<void()> step)
    {
        loadingSteps.emplace_back(std::move(label), std::move(step));
    };

    AddLoadingStep("Main menu scene", [&]()
    {
        sceneManager.AddScene(SceneInfo(new SceneMain(&sceneManager), "main", 0));
    });
    AddLoadingStep("Angry Balls menu scene", [&]()
    {
        sceneManager.AddScene(SceneInfo(new AngryBalls(&sceneManager), "angryballs_menu", 1));
    });
    AddLoadingStep("Dino Jump scene", [&]()
    {
        sceneManager.AddScene(SceneInfo(new DinoJump(&sceneManager), "dinojump", 2));
    });
    AddLoadingStep("Angry Balls level 1 scene", [&]()
    {
        sceneManager.AddScene(SceneInfo(new AngryBalls_Level1(&sceneManager), "angryballs_level1", 3));
    });
    AddLoadingStep("Angry Balls level 2 scene", [&]()
    {
        sceneManager.AddScene(SceneInfo(new AngryBalls_Level2(&sceneManager), "angryballs_level2", 4));
    });
    AddLoadingStep("Angry Balls level 3 scene", [&]()
    {
        sceneManager.AddScene(SceneInfo(new AngryBalls_Level3(&sceneManager), "angryballs_level3", 5));
    });
    AddLoadingStep("Angry Balls level 4 scene", [&]()
    {
        sceneManager.AddScene(SceneInfo(new AngryBalls_Level4(&sceneManager), "angryballs_level4", 6));
    });
    AddLoadingStep("Space Invaders scene", [&]()
    {
        sceneManager.AddScene(SceneInfo(new SpaceInvaders(&sceneManager), "space_invaders", 7));
    });
    AddLoadingStep("Where Is My Water menu scene", [&]()
    {
        sceneManager.AddScene(SceneInfo(new WhereIsMyWater(&sceneManager), "where_is_my_water_menu", 8));
    });
    AddLoadingStep("Where Is My Water level 1 scene", [&]()
    {
        sceneManager.AddScene(SceneInfo(new WhereIsMyWater_Level1(&sceneManager), "where_is_my_water_level1", 9));
    });
    AddLoadingStep("Where Is My Water level 2 scene", [&]()
    {
        sceneManager.AddScene(SceneInfo(new WhereIsMyWater_Level2(&sceneManager), "where_is_my_water_level2", 10));
    });
    AddLoadingStep("Where Is My Water level 3 scene", [&]()
    {
        sceneManager.AddScene(SceneInfo(new WhereIsMyWater_Level3(&sceneManager), "where_is_my_water_level3", 11));
    });
    AddLoadingStep("Where Is My Water level 4 scene", [&]()
    {
        sceneManager.AddScene(SceneInfo(new WhereIsMyWater_Level4(&sceneManager), "where_is_my_water_level4", 12));
    });

    AddLoadingStep("Dino Jump loading scene", [&]()
    {
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
    });

    AddLoadingStep("Space Invaders loading scene", [&]()
    {
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
    });

    AddLoadingStep("Angry Balls level 1 loading scene", [&]()
    {
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
    });

    AddLoadingStep("Angry Balls level 2 loading scene", [&]()
    {
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
    });

    AddLoadingStep("Angry Balls level 3 loading scene", [&]()
    {
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
    });

    AddLoadingStep("Angry Balls level 4 loading scene", [&]()
    {
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
    });

    AddLoadingStep("Where Is My Water level 1 loading scene", [&]()
    {
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
    });

    AddLoadingStep("Where Is My Water level 2 loading scene", [&]()
    {
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
    });

    AddLoadingStep("Where Is My Water level 3 loading scene", [&]()
    {
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
    });

    AddLoadingStep("Where Is My Water level 4 loading scene", [&]()
    {
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
    });

    size_t completedSteps = 0;
    captureStartupLogs = true;

    while (!WindowShouldClose())
    {
        const bool finished = completedSteps >= loadingSteps.size();
        std::string currentStep = finished
            ? "All startup tasks finished"
            : loadingSteps[completedSteps].first;

        BeginDrawing();
        DrawStartupLoader(completedSteps, loadingSteps.size(), currentStep);
        EndDrawing();

        if (finished && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)))
            break;

        if (!finished)
        {
            loadingSteps[completedSteps].second();
            ++completedSteps;
        }
    }

    captureStartupLogs = false;
    if (WindowShouldClose())
    {
        ResourceManager::Instance().Clear();
        CloseWindow();
        return 0;
    }

    sceneManager.LoadScene(0);

    while (!WindowShouldClose() && !sceneManager.IsQuitRequested())
    {
        float dt = GetFrameTime();

        SceneInfo current = sceneManager.GetCurrentScene();
        if (!current.scene)
            break;

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
