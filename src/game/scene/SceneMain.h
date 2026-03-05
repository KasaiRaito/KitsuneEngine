//
// Created by Eduardo Huerta on 22/01/26.
//

#ifndef KITSUNEENGINE_SCENEMAIN_H
#define KITSUNEENGINE_SCENEMAIN_H

#pragma once
#include "SceneBase.h"
#include "ResourceManager.h"
#include <vector>
#include <string>

class SceneManager;

class SceneMain : public SceneBase
{
public:
    explicit SceneMain(SceneManager* manager);
    ~SceneMain() override;

    void Update(float dt) override;
    void Draw() override;

    List<Object*> objects;

private:
    SceneManager* sceneManager = nullptr;
    std::shared_ptr<ResourceManager::FontResource> uiFont;

    std::vector<std::shared_ptr<ResourceManager::TextureResource>> angryPreviewFrames;
    std::vector<std::shared_ptr<ResourceManager::TextureResource>> dinoPreviewFrames;
    std::vector<std::shared_ptr<ResourceManager::TextureResource>> spacePreviewFrames;
    float previewFps = 14.0f;

    std::vector<std::shared_ptr<ResourceManager::TextureResource>> LoadPreviewFrames(const std::string& repoRelativeDirectory);
    void DrawPreviewPanel(const Rectangle& panelRect,
                          const std::vector<std::shared_ptr<ResourceManager::TextureResource>>& frames,
                          const char* title,
                          const char* emptyHint) const;
};


#endif //KITSUNEENGINE_SCENEMAIN_H
