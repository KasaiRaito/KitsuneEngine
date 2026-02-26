//
// Created by Eduardo Huerta on 22/01/26.
//

#ifndef KITSUNEENGINE_ANGRYBALLS_H
#define KITSUNEENGINE_ANGRYBALLS_H

#pragma once

#include "SceneBase.h"
#include "ResourceManager.h"

#include <memory>
#include <string>

class SceneManager;

class AngryBalls : public SceneBase
{
public:
    explicit AngryBalls(SceneManager* manager);
    ~AngryBalls() override = default;

    void Update(float dt) override;
    void Draw() override;

private:
    enum class AngrySceneId
    {
        Menu = 0,
        Level1 = 1,
        Level2 = 2,
        Level3 = 3,
        Level4 = 4
    };

    static constexpr int kProjectMainMenuSceneIndex = 0;
    static constexpr int kAngryLevel1SceneIndex = 3;
    static constexpr int kAngryLevel2SceneIndex = 4;
    static constexpr int kAngryLevel3SceneIndex = 5;
    static constexpr int kAngryLevel4SceneIndex = 6;
    static constexpr int kAngryTotalLevels = 4;

    SceneManager* sceneManager = nullptr;
    std::shared_ptr<ResourceManager::FontResource> uiFont;
    std::shared_ptr<ResourceManager::TextureResource> backgroundImage;

    int unlockedLevelCount = 1;

    void RefreshProgressFromSave();
    void StartAngryScene(AngrySceneId targetScene);

    static std::string ResolveAssetPath(const std::string& repoRelativePath);
};

#endif // KITSUNEENGINE_ANGRYBALLS_H
