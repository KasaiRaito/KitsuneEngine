#include "WhereIsMyWater_Level3.h"

WhereIsMyWater_Level3::WhereIsMyWater_Level3(SceneManager* manager)
    : WhereIsMyWaterLevelBase(manager)
{
}

int WhereIsMyWater_Level3::GetLevelNumber() const
{
    return 3;
}

const char* WhereIsMyWater_Level3::GetLevelTitle() const
{
    return "Where Is My Water? - Level 3";
}

int WhereIsMyWater_Level3::GetNextLevelSceneIndex() const
{
    return 12;
}
