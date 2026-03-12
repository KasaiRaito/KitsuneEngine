#include "WhereIsMyWater_Level1.h"

WhereIsMyWater_Level1::WhereIsMyWater_Level1(SceneManager* manager)
    : WhereIsMyWaterLevelBase(manager)
{
}

int WhereIsMyWater_Level1::GetLevelNumber() const
{
    return 1;
}

const char* WhereIsMyWater_Level1::GetLevelTitle() const
{
    return "Where Is My Water? - Level 1";
}

int WhereIsMyWater_Level1::GetNextLevelSceneIndex() const
{
    return 20;
}
