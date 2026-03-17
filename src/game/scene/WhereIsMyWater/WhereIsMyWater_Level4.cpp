#include "WhereIsMyWater_Level4.h"

WhereIsMyWater_Level4::WhereIsMyWater_Level4(SceneManager* manager)
    : WhereIsMyWaterLevelBase(manager)
{
}

int WhereIsMyWater_Level4::GetLevelNumber() const
{
    return 4;
}

const char* WhereIsMyWater_Level4::GetLevelTitle() const
{
    return "Where Is My Water? - Level 4";
}

int WhereIsMyWater_Level4::GetNextLevelSceneIndex() const
{
    return -1;
}
