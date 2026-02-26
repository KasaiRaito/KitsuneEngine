#include "AngryBalls/AngryBalls.h"

#include "SceneManager.h"
#include "save_data/SaveData.h"

#include "raylib.h"
#include "raygui.h"

#include <algorithm>

std::string AngryBalls::ResolveAssetPath(const std::string& repoRelativePath)
{
    const std::string candidates[] = {
        repoRelativePath,
        "../" + repoRelativePath,
        "../../" + repoRelativePath,
        "../../../" + repoRelativePath
    };

    for (const std::string& candidate : candidates)
    {
        if (FileExists(candidate.c_str()))
            return candidate;
    }

    return repoRelativePath;
}

AngryBalls::AngryBalls(SceneManager* manager)
    : sceneManager(manager)
{
    auto& resources = ResourceManager::Instance();
    const std::string fontPath = ResolveAssetPath("src/game/assets/fonts/DKKitsuneTail.ttf");
    const std::string backgroundPath = ResolveAssetPath("src/game/assets/textures/AngryBallsBackground.png");

    uiFont = resources.GetOrLoadFont(fontPath);
    backgroundImage = resources.GetOrLoadTexture(backgroundPath);

    RefreshProgressFromSave();
    SaveData::Instance().SetAngryCurrentScene((int)AngrySceneId::Menu);
}

void AngryBalls::RefreshProgressFromSave()
{
    unlockedLevelCount = std::clamp(SaveData::Instance().GetAngryUnlockedLevelCount(), 1, 2);
}

void AngryBalls::StartAngryScene(AngrySceneId targetScene)
{
    if (!sceneManager)
        return;

    int targetSceneIndex = -1;
    switch (targetScene)
    {
    case AngrySceneId::Level1:
        targetSceneIndex = kAngryLevel1SceneIndex;
        break;
    case AngrySceneId::Level2:
        if (unlockedLevelCount < 2)
            return;
        targetSceneIndex = kAngryLevel2SceneIndex;
        break;
    case AngrySceneId::Menu:
    default:
        targetSceneIndex = 1;
        break;
    }

    SaveData::Instance().SetAngryCurrentScene((int)targetScene);
    sceneManager->LoadScene(targetSceneIndex);
}

void AngryBalls::Update(float dt)
{
    (void)dt;
    RefreshProgressFromSave();
    SaveData::Instance().SetAngryCurrentScene((int)AngrySceneId::Menu);
}

void AngryBalls::Draw()
{
    if (backgroundImage && backgroundImage->value.id != 0)
    {
        Texture2D& bg = backgroundImage->value;
        DrawTexturePro(
            bg,
            { 0.0f, 0.0f, (float)bg.width, (float)bg.height },
            { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() },
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }
    else
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{ 206, 226, 255, 255 });
    }

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.25f));

    if (uiFont)
        DrawTextEx(uiFont->value, "AngryBalls", { 26.0f, 20.0f }, 52.0f, 1.0f, WHITE);
    else
        DrawText("AngryBalls", 26, 20, 52, WHITE);

    DrawText("Game Menu", 30, 82, 34, WHITE);
    DrawText("Select a level to play", 30, 118, 24, Color{ 240, 240, 240, 255 });
    DrawText(TextFormat("Unlocked levels: %d/2", unlockedLevelCount), 30, 146, 24, Color{ 255, 232, 156, 255 });

    const int currentSceneId = SaveData::Instance().GetAngryCurrentScene();
    const char* currentSceneLabel = "AngryBalls_Menu";
    if (currentSceneId == (int)AngrySceneId::Level1)
        currentSceneLabel = "AngryBalls_Level1";
    else if (currentSceneId == (int)AngrySceneId::Level2)
        currentSceneLabel = "AngryBalls_Level2";

    DrawText(TextFormat("Current scene: %s", currentSceneLabel), 30, 174, 22, Color{ 239, 239, 239, 255 });

    const float buttonWidth = 280.0f;
    const float buttonHeight = 64.0f;
    const float buttonSpacing = 22.0f;
    const float totalWidth = (buttonWidth * 2.0f) + buttonSpacing;
    const float startX = ((float)GetScreenWidth() - totalWidth) * 0.5f;
    const float y = (float)GetScreenHeight() * 0.55f;

    const Rectangle level1Button = { startX, y, buttonWidth, buttonHeight };
    const Rectangle level2Button = { startX + buttonWidth + buttonSpacing, y, buttonWidth, buttonHeight };

    if (GuiButton(level1Button, "AngryBalls_Level1"))
        StartAngryScene(AngrySceneId::Level1);

    if (unlockedLevelCount >= 2)
    {
        if (GuiButton(level2Button, "AngryBalls_Level2"))
            StartAngryScene(AngrySceneId::Level2);
    }
    else
    {
        DrawRectangleRounded(level2Button, 0.22f, 8, Fade(BLACK, 0.55f));
        DrawRectangleRoundedLinesEx(level2Button, 0.22f, 8, 2.0f, Fade(WHITE, 0.45f));

        const char* label = "AngryBalls_Level2";
        const int labelSize = 27;
        const int labelW = MeasureText(label, labelSize);
        DrawText(
            label,
            (int)(level2Button.x + (level2Button.width - (float)labelW) * 0.5f),
            (int)level2Button.y + 12,
            labelSize,
            Fade(WHITE, 0.7f)
        );

        const char* lockText = "Locked - clear Level1";
        const int lockW = MeasureText(lockText, 20);
        DrawText(
            lockText,
            (int)(level2Button.x + (level2Button.width - (float)lockW) * 0.5f),
            (int)(level2Button.y + level2Button.height + 8.0f),
            20,
            Color{ 255, 210, 160, 255 }
        );
    }

    if (GuiButton({ 20, (float)GetScreenHeight() - 52, 190, 34 }, "Back To Project Menu") && sceneManager)
        sceneManager->LoadScene(kProjectMainMenuSceneIndex);

    DrawText("Level2 unlocks after Level1 is cleared. Progress is stored in save_data.json", 20, GetScreenHeight() - 82, 18, WHITE);
}
