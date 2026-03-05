#include "WhereIsMyWater_Level2.h"

WhereIsMyWater_Level2::WhereIsMyWater_Level2(SceneManager* manager)
    : WhereIsMyWaterLevelBase(manager)
{
}

int WhereIsMyWater_Level2::GetLevelNumber() const
{
    return 2;
}

const char* WhereIsMyWater_Level2::GetLevelTitle() const
{
    return "Where Is My Water? - Level 2";
}

int WhereIsMyWater_Level2::GetNextLevelSceneIndex() const
{
    return 21;
}
