//
// Created by Eduardo Huerta on 05/03/26.
//

#ifndef KITSUNEENGINE_WHEREISMYWATER_H
#define KITSUNEENGINE_WHEREISMYWATER_H

#pragma once

#include "SceneBase.h"
#include "ResourceManager.h"

#include <memory>
#include <string>

class SceneManager;

class WhereIsMyWater : public SceneBase
{
public:
    explicit WhereIsMyWater(SceneManager* manager);
    ~WhereIsMyWater() override = default;

    void Update(float dt) override;
    void Draw() override;

private:
    static constexpr int kProjectMainMenuSceneIndex = 0;
    static constexpr int kWaterLevel1SceneIndex = 9;
    static constexpr int kWaterLevel2SceneIndex = 10;
    static constexpr int kWaterLevel3SceneIndex = 11;
    static constexpr int kWaterLevel4SceneIndex = 12;
    static constexpr int kWaterTotalLevels = 4;

    SceneManager* sceneManager = nullptr;
    std::shared_ptr<ResourceManager::FontResource> uiFont;

    int unlockedLevelCount = 1;

    static std::string ResolveAssetPath(const std::string& repoRelativePath);
    void RefreshProgressFromSave();
    void StartWaterScene(int levelNumber);
};

#endif // KITSUNEENGINE_WHEREISMYWATER_H
