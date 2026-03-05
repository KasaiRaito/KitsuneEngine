#pragma once

#include "SceneBase.h"

#include <string>
#include <vector>

class SceneManager;

class GameLoadingScene : public SceneBase
{
public:
    GameLoadingScene(
        SceneManager* manager,
        int nextSceneIndex,
        std::string gameTitle,
        std::vector<std::string> controls,
        float minDurationSeconds = 2.40f,
        float minDisplaySeconds = 0.85f
    );
    ~GameLoadingScene() override = default;

    void Update(float dt) override;
    void Draw() override;

private:
    void TransitionToTarget();

    SceneManager* sceneManager = nullptr;
    int targetSceneIndex = -1;
    std::string title;
    std::vector<std::string> controlLines;
    float minimumDuration = 2.40f;
    float minimumDisplay = 0.85f;
    float elapsedSeconds = 0.0f;
};
