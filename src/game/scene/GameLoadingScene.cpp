#include "GameLoadingScene.h"

#include "InputSystem.h"
#include "SceneManager.h"

#include "raylib.h"

#include <algorithm>
#include <utility>

GameLoadingScene::GameLoadingScene(
    SceneManager* manager,
    int nextSceneIndex,
    std::string gameTitle,
    std::vector<std::string> controls,
    float minDurationSeconds,
    float minDisplaySeconds
)
    : sceneManager(manager)
    , targetSceneIndex(nextSceneIndex)
    , title(std::move(gameTitle))
    , controlLines(std::move(controls))
    , minimumDuration(std::max(0.1f, minDurationSeconds))
    , minimumDisplay(std::max(0.0f, minDisplaySeconds))
{
}

void GameLoadingScene::TransitionToTarget()
{
    if (!sceneManager || targetSceneIndex < 0)
        return;

    // Reset timer so this scene behaves correctly every time it is reopened.
    elapsedSeconds = 0.0f;
    sceneManager->LoadScene(targetSceneIndex);
}

void GameLoadingScene::Update(float dt)
{
    if (dt > 0.0f)
        elapsedSeconds += dt;

    const bool finishedLoading = elapsedSeconds >= minimumDuration;
    const bool canConfirm = elapsedSeconds >= std::max(minimumDuration, minimumDisplay);
    const bool confirmRequested = InputSystem::IsAnyKeyPressed({ KEY_SPACE, KEY_ENTER });

    if (finishedLoading && canConfirm && confirmRequested)
        TransitionToTarget();
}

void GameLoadingScene::Draw()
{
    const int screenW = GetScreenWidth();
    const int screenH = GetScreenHeight();

    DrawRectangleGradientV(
        0,
        0,
        screenW,
        screenH,
        Color{ 24, 32, 48, 255 },
        Color{ 10, 14, 22, 255 }
    );

    const float panelWidth = std::min(720.0f, (float)screenW - 52.0f);
    const float panelHeight = std::min(360.0f, (float)screenH - 72.0f);
    const Rectangle panel = {
        ((float)screenW - panelWidth) * 0.5f,
        ((float)screenH - panelHeight) * 0.5f,
        panelWidth,
        panelHeight
    };

    DrawRectangleRounded(panel, 0.08f, 10, Fade(BLACK, 0.55f));
    DrawRectangleRoundedLinesEx(panel, 0.08f, 10, 2.0f, Fade(RAYWHITE, 0.55f));

    const char* titleText = title.empty() ? "Loading Game" : title.c_str();
    DrawText(
        titleText,
        (int)panel.x + 24,
        (int)panel.y + 20,
        38,
        RAYWHITE
    );
    DrawText(
        "Controls",
        (int)panel.x + 28,
        (int)panel.y + 72,
        28,
        Color{ 180, 232, 255, 255 }
    );

    int lineY = (int)panel.y + 110;
    if (controlLines.empty())
    {
        DrawText(
            "No controls configured for this game.",
            (int)panel.x + 32,
            lineY,
            23,
            LIGHTGRAY
        );
    }
    else
    {
        for (size_t i = 0; i < controlLines.size(); ++i)
        {
            DrawText(
                TextFormat("%d. %s", (int)i + 1, controlLines[i].c_str()),
                (int)panel.x + 32,
                lineY,
                23,
                RAYWHITE
            );
            lineY += 32;
        }
    }

    const float progressRatio = std::clamp(elapsedSeconds / minimumDuration, 0.0f, 1.0f);
    const Rectangle barBackground = {
        panel.x + 28.0f,
        panel.y + panel.height - 70.0f,
        panel.width - 56.0f,
        24.0f
    };

    DrawRectangleRounded(barBackground, 0.35f, 8, Fade(WHITE, 0.16f));

    Rectangle barFill = barBackground;
    barFill.width *= progressRatio;
    if (barFill.width > 1.0f)
        DrawRectangleRounded(barFill, 0.35f, 8, Color{ 122, 219, 255, 255 });

    const bool finishedLoading = elapsedSeconds >= minimumDuration;
    const bool canConfirm = elapsedSeconds >= std::max(minimumDuration, minimumDisplay);

    DrawText(
        finishedLoading ? "Loaded" : "Loading...",
        (int)barBackground.x + 8,
        (int)barBackground.y - 24,
        20,
        Color{ 214, 233, 245, 255 }
    );

    if (canConfirm)
    {
        const char* continueHint = "Press \"Space\" or \"Enter\" to continue.";
        const int hintWidth = MeasureText(continueHint, 20);
        DrawText(
            continueHint,
            (screenW - hintWidth) / 2,
            (int)(panel.y + panel.height + 18.0f),
            20,
            Color{ 199, 207, 219, 255 }
        );
    }
    else
    {
        const char* continueHint = "Please wait...";
        const int hintWidth = MeasureText(continueHint, 20);
        DrawText(
            continueHint,
            (screenW - hintWidth) / 2,
            (int)(panel.y + panel.height + 18.0f),
            20,
            Color{ 199, 207, 219, 255 }
        );
    }
}
