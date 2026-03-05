//
// Created by Eduardo Huerta on 05/03/26.
//

#include "WhereIsMyWater.h"

#include "SceneManager.h"
#include "save_data/SaveData.h"

#include "raylib.h"
#include "raygui.h"

#include <algorithm>

std::string WhereIsMyWater::ResolveAssetPath(const std::string& repoRelativePath)
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

WhereIsMyWater::WhereIsMyWater(SceneManager* manager)
    : sceneManager(manager)
{
    auto& resources = ResourceManager::Instance();
    const std::string fontPath = ResolveAssetPath("src/game/assets/fonts/DKKitsuneTail.ttf");
    uiFont = resources.GetOrLoadFont(fontPath);

    RefreshProgressFromSave();
}

void WhereIsMyWater::RefreshProgressFromSave()
{
    unlockedLevelCount = std::clamp(SaveData::Instance().GetWaterUnlockedLevelCount(), 1, kWaterTotalLevels);
}

void WhereIsMyWater::StartWaterScene(int levelNumber)
{
    if (!sceneManager)
        return;

    RefreshProgressFromSave();

    const int clampedLevel = std::clamp(levelNumber, 1, kWaterTotalLevels);
    if (clampedLevel > unlockedLevelCount)
        return;

    SaveData::Instance().SetWaterCurrentLevel(clampedLevel);

    int sceneIndex = kWaterLevel1SceneIndex;
    if (clampedLevel == 2)
        sceneIndex = kWaterLevel2SceneIndex;
    else if (clampedLevel == 3)
        sceneIndex = kWaterLevel3SceneIndex;
    else if (clampedLevel == 4)
        sceneIndex = kWaterLevel4SceneIndex;

    sceneManager->LoadScene(sceneIndex);
}

void WhereIsMyWater::Update(float dt)
{
    (void)dt;
    RefreshProgressFromSave();
}

void WhereIsMyWater::Draw()
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{ 19, 28, 47, 255 });

    if (uiFont)
        DrawTextEx(uiFont->value, "Where Is My Water?", { 20.0f, 16.0f }, 40.0f, 1.0f, RAYWHITE);
    else
        DrawText("Where Is My Water?", 20, 16, 40, RAYWHITE);

    DrawText("Level Select", 24, 62, 30, Color{ 187, 232, 255, 255 });
    DrawText(
        TextFormat("Unlocked levels: %d/%d", unlockedLevelCount, kWaterTotalLevels),
        24,
        92,
        22,
        Color{ 255, 234, 165, 255 }
    );

    const float buttonWidth = 280.0f;
    const float buttonHeight = 74.0f;
    const float columnSpacing = 28.0f;
    const float rowSpacing = 26.0f;
    const float totalWidth = (buttonWidth * 2.0f) + columnSpacing;
    const float startX = ((float)GetScreenWidth() - totalWidth) * 0.5f;
    const float startY = 152.0f;

    for (int levelNumber = 1; levelNumber <= kWaterTotalLevels; ++levelNumber)
    {
        const int index = levelNumber - 1;
        const int column = index % 2;
        const int row = index / 2;

        const Rectangle buttonRect = {
            startX + ((buttonWidth + columnSpacing) * (float)column),
            startY + ((buttonHeight + rowSpacing) * (float)row),
            buttonWidth,
            buttonHeight
        };

        const bool unlocked = levelNumber <= unlockedLevelCount;
        const int bestScore = SaveData::Instance().GetWaterBestScore(levelNumber);

        if (unlocked)
        {
            if (GuiButton(buttonRect, TextFormat("WhereIsMyWater_Level%d", levelNumber)))
                StartWaterScene(levelNumber);

            DrawText(
                TextFormat("Best collected: %d", bestScore),
                (int)buttonRect.x + 12,
                (int)buttonRect.y + 45,
                18,
                Color{ 194, 231, 255, 255 }
            );
            continue;
        }

        DrawRectangleRounded(buttonRect, 0.22f, 8, Fade(BLACK, 0.60f));
        DrawRectangleRoundedLinesEx(buttonRect, 0.22f, 8, 2.0f, Fade(WHITE, 0.45f));

        DrawText(
            TextFormat("WhereIsMyWater_Level%d", levelNumber),
            (int)buttonRect.x + 12,
            (int)buttonRect.y + 14,
            24,
            Fade(WHITE, 0.74f)
        );
        DrawText(
            TextFormat("Locked - clear Level %d", levelNumber - 1),
            (int)buttonRect.x + 12,
            (int)buttonRect.y + 45,
            18,
            Color{ 255, 198, 170, 255 }
        );
    }

    if (GuiButton({ 20.0f, (float)GetScreenHeight() - 52.0f, 200.0f, 34.0f }, "Back To Project Menu") && sceneManager)
        sceneManager->LoadScene(kProjectMainMenuSceneIndex);

    DrawText(
        "Each cleared level unlocks the next one. Progress is saved in save_data.json",
        20,
        GetScreenHeight() - 82,
        18,
        RAYWHITE
    );
}
