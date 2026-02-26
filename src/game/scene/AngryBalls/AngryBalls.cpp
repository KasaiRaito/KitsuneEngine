#include "AngryBalls/AngryBalls.h"

#include "AngryBalls_LevelBase.h"
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
    unlockedLevelCount = std::clamp(SaveData::Instance().GetAngryUnlockedLevelCount(), 1, kAngryTotalLevels);
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
    case AngrySceneId::Level3:
        if (unlockedLevelCount < 3)
            return;
        targetSceneIndex = kAngryLevel3SceneIndex;
        break;
    case AngrySceneId::Level4:
        if (unlockedLevelCount < 4)
            return;
        targetSceneIndex = kAngryLevel4SceneIndex;
        break;
    case AngrySceneId::Menu:
    default:
        targetSceneIndex = 1;
        break;
    }

    if (targetScene != AngrySceneId::Menu)
        AngryBallsLevelBase::RequestResetForLevel((int)targetScene);

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
        DrawTextEx(uiFont->value, "Angry-Balls", { 26.0f, 20.0f }, 52.0f, 1.0f, WHITE);
    else
        DrawText("Angry-Balls", 26, 20, 52, WHITE);

    DrawText("Game Menu", 30, 82, 34, WHITE);
    DrawText("Select a level to play", 30, 118, 24, Color{ 240, 240, 240, 255 });
    DrawText(TextFormat("Unlocked levels: %d/%d", unlockedLevelCount, kAngryTotalLevels), 30, 146, 24, Color{ 255, 232, 156, 255 });

    const int currentSceneId = SaveData::Instance().GetAngryCurrentScene();
    const char* currentSceneLabel = "AngryBalls_Menu";
    if (currentSceneId == (int)AngrySceneId::Level1)
        currentSceneLabel = "AngryBalls_Level1";
    else if (currentSceneId == (int)AngrySceneId::Level2)
        currentSceneLabel = "AngryBalls_Level2";
    else if (currentSceneId == (int)AngrySceneId::Level3)
        currentSceneLabel = "AngryBalls_Level3";
    else if (currentSceneId == (int)AngrySceneId::Level4)
        currentSceneLabel = "AngryBalls_Level4";

    DrawText(TextFormat("Current scene: %s", currentSceneLabel), 30, 174, 22, Color{ 239, 239, 239, 255 });

    const float buttonWidth = 280.0f;
    const float buttonHeight = 56.0f;
    const float columnSpacing = 22.0f;
    const float rowSpacing = 26.0f;
    const float totalWidth = (buttonWidth * 2.0f) + columnSpacing;
    const float startX = ((float)GetScreenWidth() - totalWidth) * 0.5f;
    const float startY = (float)GetScreenHeight() * 0.43f;

    struct LevelButton
    {
        AngrySceneId sceneId;
        int levelNumber;
        const char* label;
    };

    const LevelButton buttons[] = {
        { AngrySceneId::Level1, 1, "AngryBalls_Level1" },
        { AngrySceneId::Level2, 2, "AngryBalls_Level2" },
        { AngrySceneId::Level3, 3, "AngryBalls_Level3" },
        { AngrySceneId::Level4, 4, "AngryBalls_Level4" }
    };

    for (int i = 0; i < 4; ++i)
    {
        const int column = i % 2;
        const int row = i / 2;

        const Rectangle buttonRect = {
            startX + ((buttonWidth + columnSpacing) * (float)column),
            startY + ((buttonHeight + rowSpacing) * (float)row),
            buttonWidth,
            buttonHeight
        };

        const LevelButton& entry = buttons[i];
        if (unlockedLevelCount >= entry.levelNumber)
        {
            if (GuiButton(buttonRect, entry.label))
                StartAngryScene(entry.sceneId);
            continue;
        }

        DrawRectangleRounded(buttonRect, 0.22f, 8, Fade(BLACK, 0.55f));
        DrawRectangleRoundedLinesEx(buttonRect, 0.22f, 8, 2.0f, Fade(WHITE, 0.45f));

        const int labelSize = 24;
        const int labelW = MeasureText(entry.label, labelSize);
        DrawText(
            entry.label,
            (int)(buttonRect.x + (buttonRect.width - (float)labelW) * 0.5f),
            (int)buttonRect.y + 10,
            labelSize,
            Fade(WHITE, 0.7f)
        );

        const char* lockText = TextFormat("Locked - clear Level%d", entry.levelNumber - 1);
        const int lockW = MeasureText(lockText, 18);
        DrawText(
            lockText,
            (int)(buttonRect.x + (buttonRect.width - (float)lockW) * 0.5f),
            (int)(buttonRect.y + buttonRect.height + 6.0f),
            18,
            Color{ 255, 210, 160, 255 }
        );
    }

    if (GuiButton({ 20, (float)GetScreenHeight() - 52, 190, 34 }, "Back To Project Menu") && sceneManager)
        sceneManager->LoadScene(kProjectMainMenuSceneIndex);

    DrawText("Each cleared level unlocks the next one. Progress is stored in save_data.json", 20, GetScreenHeight() - 82, 18, WHITE);
}
