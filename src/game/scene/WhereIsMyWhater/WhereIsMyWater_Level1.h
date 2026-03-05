#pragma once

#include "WhereIsMyWater_LevelBase.h"

class SceneManager;

class WhereIsMyWater_Level1 : public WhereIsMyWaterLevelBase
{
public:
    explicit WhereIsMyWater_Level1(SceneManager* manager);
    ~WhereIsMyWater_Level1() override = default;

protected:
    int GetLevelNumber() const override;
    const char* GetLevelTitle() const override;
    int GetNextLevelSceneIndex() const override;
};
