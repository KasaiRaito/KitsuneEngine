//
// Created by Eduardo Huerta on 22/01/26.
//

#ifndef KITSUNEENGINE_SCENEMANAGER_H
#define KITSUNEENGINE_SCENEMANAGER_H

#pragma once
#include "List.h"
#include "SceneBase.h"
#include <string>

#pragma once
#include "List.h"
#include "SceneBase.h"
#include <string>

struct SceneInfo
{
    SceneBase* scene = nullptr;
    std::string name;
    int index = -1;

    SceneInfo() = default;
    SceneInfo(SceneBase* s, std::string sceneName, int sceneIndex)
        : scene(s), name(std::move(sceneName)), index(sceneIndex) {}
};

class SceneManager
{
public:
    SceneManager();

    void AddScene(const SceneInfo& scene);

    SceneInfo GetCurrentScene() const;
    int GetCurrentSceneIndex() const;

    SceneInfo* GetNextScene();
    void LoadScene(int index);

    void NextScene();

private:
    List<SceneInfo> scenes;
    int currentSceneIndex = -1;
};


#endif //KITSUNEENGINE_SCENEMANAGER_H