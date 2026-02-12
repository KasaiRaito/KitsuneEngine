//
// Created by Eduardo Huerta on 22/01/26.
//

#ifndef KITSUNEENGINE_SCENEGAME_H
#define KITSUNEENGINE_SCENEGAME_H

#pragma once
#include "SceneBase.h"
#include "List.h"
#include "Object.h"
#include "resource_manager/ResourceManager.h"
#include <memory>

class SceneGame : public SceneBase
{
public:
    SceneGame();
    ~SceneGame() override;

    void Update(float dt) override;
    void Draw() override;

    void SetTime(int h, int m, int s, int ms);

private:
    List<Object*> objects;

    // Cached resources loaded once through ResourceManager.
    std::shared_ptr<ResourceManager::FontResource> uiFont;
    std::shared_ptr<ResourceManager::SoundResource> collisionSfx;
    std::shared_ptr<ResourceManager::MusicResource> bgm;
    std::shared_ptr<ResourceManager::TextureResource> backgroundImage;

    void UpdateClock(float dt, bool grow);
    void GetClockParts(int& h, int& m, int& s, int& ms) const;

    int totalMs;
    const int dayMs = 24 * 60 * 60 * 1000;
};

#endif //KITSUNEENGINE_SCENEGAME_H
