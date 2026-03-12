#pragma once

#include "WhereIsMyWater_LevelBase.h"

class SceneManager;

class WhereIsMyWater_Level3 : public WhereIsMyWaterLevelBase
{
public:
    explicit WhereIsMyWater_Level3(SceneManager* manager);
    ~WhereIsMyWater_Level3() override = default;

protected:
    int GetLevelNumber() const override;
    const char* GetLevelTitle() const override;
    int GetNextLevelSceneIndex() const override;
};
