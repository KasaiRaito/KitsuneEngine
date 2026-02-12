//
// Created by Eduardo Huerta on 22/01/26.
//

#ifndef KITSUNEENGINE_SCENEMAIN_H
#define KITSUNEENGINE_SCENEMAIN_H

#pragma once
#include "SceneBase.h"
#include "ResourceManager.h"

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
};


#endif //KITSUNEENGINE_SCENEMAIN_H
