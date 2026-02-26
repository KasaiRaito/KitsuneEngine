//
// Created by Eduardo Huerta on 26/02/26.
//

#include "AngryBalls_Level3.h"

AngryBalls_Level3::AngryBalls_Level3(SceneManager* manager)
    : AngryBallsLevelBase(manager)
{
}

void AngryBalls_Level3::BuildLevelLayout()
{
    if (LoadLevelLayoutFromJson("src/game/assets/levels/angryballs_level3.json"))
        return;
}

int AngryBalls_Level3::GetBirdCount() const
{
    return 6;
}

int AngryBalls_Level3::GetLevelNumber() const
{
    return 3;
}

const char* AngryBalls_Level3::GetLevelTitle() const
{
    return "AngryBalls_Level3";
}

int AngryBalls_Level3::GetNextLevelSceneIndex() const
{
    return 6;
}
