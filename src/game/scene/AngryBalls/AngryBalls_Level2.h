//
// Created by Eduardo Huerta on 26/02/26.
//

#ifndef KITSUNEENGINE_ANGRYBALLS_LEVEL2_H
#define KITSUNEENGINE_ANGRYBALLS_LEVEL2_H

#pragma once

#include "AngryBalls_LevelBase.h"

class SceneManager;

class AngryBalls_Level2 : public AngryBallsLevelBase
{
public:
    explicit AngryBalls_Level2(SceneManager* manager);
    ~AngryBalls_Level2() override = default;

protected:
    void BuildLevelLayout() override;
    int GetBirdCount() const override;
    int GetLevelNumber() const override;
    const char* GetLevelTitle() const override;
    int GetNextLevelSceneIndex() const override;
};

#endif // KITSUNEENGINE_ANGRYBALLS_LEVEL2_H
